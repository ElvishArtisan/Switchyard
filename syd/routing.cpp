// routing.cpp
//
// Switchyard stream routing configuration
//
// (C) 2014 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
//

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "profile.h"
#include "routing.h"

Routing::Routing()
{
  sy_shm_block=NULL;
  if(!InitShmSegment(&sy_shm_block)) {
    fprintf(stderr,"unable to allocate shared memory\n");
    exit(256);
  }
  memset(sy_shm_block,0,sizeof(ShmBlock));
  LoadInterfaces();
  load();
}


QHostAddress Routing::nicAddress() const
{
  return QHostAddress(ntohl(sy_shm_block->nic_addr));
}


void Routing::setNicAddress(const QHostAddress &addr)
{
  sy_shm_block->nic_addr=htonl(addr.toIPv4Address());
}


QHostAddress Routing::srcAddress(int slot) const
{
  return QHostAddress(ntohl(sy_shm_block->src_addr[slot]));
}


void Routing::setSrcAddress(int slot,const QHostAddress &addr)
{
  sy_shm_block->src_addr[slot]=htonl(addr.toIPv4Address());
}


QHostAddress Routing::dstAddress(int slot) const
{
  return QHostAddress(ntohl(sy_shm_block->dst_addr[slot]));
}


void Routing::setDstAddress(int slot,const QHostAddress &addr)
{
  sy_shm_block->dst_addr[slot]=htonl(addr.toIPv4Address());
}


struct ShmBlock *Routing::shmBlock() const
{
  return sy_shm_block;
}


unsigned Routing::nicQuantity() const
{
  return sy_nic_devices.size();
}


QHostAddress Routing::nicAddress(unsigned n)
{
  return sy_nic_addresses[n];
}


QString Routing::nicDevice(unsigned n)
{
  return sy_nic_devices[n];
}


void Routing::load()
{
  Profile *p=new Profile();
  p->setSource(SWITCHYARD_ROUTING_FILE);
  QHostAddress default_nic;
  if(nicQuantity()>0) {
    default_nic=nicAddress(0);
  }

  setNicAddress(p->addressValue("Global","NicAddress",default_nic));
  for(int i=0;i<SWITCHYARD_SLOTS;i++) {
    setSrcAddress(i,p->addressValue(QString().sprintf("Slot%u",i+1),
				    "SourceAddress",""));
    setDstAddress(i,p->addressValue(QString().sprintf("Slot%u",i+1),
				    "DestinationAddress",""));
  }

  delete p;
}


void Routing::save() const
{
  FILE *f=NULL;
  QString tempfile=QString(SWITCHYARD_ROUTING_FILE)+"-temp";

  if((f=fopen(tempfile.toAscii(),"w"))==NULL) {
    syslog(LOG_WARNING,"unable to save routing data [%s]",strerror(errno));
    return;
  }

  fprintf(f,"[Global]\n");
  fprintf(f,"NicAddress=%s\n",(const char *)nicAddress().toString().toAscii());
  fprintf(f,"\n");
  for(int i=0;i<SWITCHYARD_SLOTS;i++) {
    fprintf(f,"[Slot%u]\n",i+1);
    fprintf(f,"SourceAddress=%s\n",
	    (const char *)srcAddress(i).toString().toAscii());
    fprintf(f,"DestinationAddress=%s\n",
	    (const char *)dstAddress(i).toString().toAscii());
    fprintf(f,"\n");
  }

  fclose(f);
  rename(tempfile.toAscii(),SWITCHYARD_ROUTING_FILE);
}


bool Routing::InitShmSegment(struct ShmBlock **sy_shm)
{
  struct shmid_ds shmid_ds;

  /*
   * First try to create a new shared memory segment.
   */
  if((sy_shm_id=
      shmget(SWITCHYARD_SHM_KEY,sizeof(struct ShmBlock),
	     IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|
	     S_IRGRP|S_IWUSR|S_IROTH|S_IWOTH))<0) {
    if(errno!=EEXIST) {
      return false;
    }
    /*
     * The shmget() error was due to an existing segment, so try to get it,
     *  release it, and re-get it.
     */
    sy_shm_id = shmget(SWITCHYARD_SHM_KEY,sizeof(struct ShmBlock),0);
    shmctl(sy_shm_id,IPC_RMID,NULL);
    if((sy_shm_id=shmget(SWITCHYARD_SHM_KEY,sizeof(struct ShmBlock),
			 IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|
			 S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))<0) {
      return false;
    }
  }
  shmctl(sy_shm_id,IPC_STAT,&shmid_ds);
  shmid_ds.shm_perm.uid=getuid();
  shmctl(sy_shm_id,IPC_SET,&shmid_ds);
  *sy_shm=(struct ShmBlock *)shmat(sy_shm_id,NULL,0);

  return true;
}


void Routing::LoadInterfaces()
{
  int fd;
  struct ifreq ifr;
  int index=0;
  uint64_t mac;

  if((fd=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    return;
  }

  memset(&ifr,0,sizeof(ifr));
  index=1;
  ifr.ifr_ifindex=index;
  while(ioctl(fd,SIOCGIFNAME,&ifr)==0) {
    if(ioctl(fd,SIOCGIFHWADDR,&ifr)==0) {
      mac=
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[0])<<40)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[1])<<32)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[2])<<24)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[3])<<16)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[4])<<8)+
	(0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[5]);
      if(mac!=0) {
	sy_nic_devices.
	  push_back(QString().
		    sprintf("%s: %02X:%02X:%02X:%02X:%02X:%02X",
			    ifr.ifr_name,
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[0],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[1],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[2],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[3],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[4],
			    0xff&ifr.ifr_ifru.ifru_hwaddr.sa_data[5]));
	sy_nic_addresses.push_back(QHostAddress());
	if(ioctl(fd,SIOCGIFADDR,&ifr)==0) {
	  struct sockaddr_in sa=*(sockaddr_in *)(&ifr.ifr_addr);
	  sy_nic_addresses.back().setAddress(ntohl(sa.sin_addr.s_addr));
	}
      }
    }
    ifr.ifr_ifindex=++index;
  }
  close(fd);

}
