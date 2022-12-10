#include <iostream>
#include <memory>

#include "uhal/uhal.hpp"

// Default XML connection file to be used
#define DEFAULT_CONNECTION_FILE "file://opt/address_table/connections.xml"

// Default register names to read
#define DEFUALT_LOCAL_REGISTER  "PL_MEM.ARM.CPU_LOAD"
#define DEFAULT_REMOTE_REGISTER "CM_V_INFO.GIT_HASH_1" 
#define DEFAULT_MMAP_REGISTER   "info.magic"

int main(int argc, char* argv[])
{
  // Connection file to use: Path is relative to the bin/main executable
  // This connection file has two set of hardware interfaces:
  // 1. UIOuHAL interface, 2. MemMap interface
  std::string connectionFile(DEFAULT_CONNECTION_FILE);

  std::cout << "Using connection file: " << connectionFile << std::endl;

  // Get the connection manager
  uhal::setLogLevelTo(uhal::Notice());
  uhal::ConnectionManager manager( connectionFile.c_str(), std::vector<std::string>(1,"uioaxi-1.0") ); 
  std::cout << "Constructed ConnectionManager" << std::endl;

  // Get device interface for UIOuHAL 
  std::cout << "Getting HW interface for UIOuHAL" << std::endl;
  std::string connectionFileEntryUIO("test.0");
  uhal::HwInterface * hw_uio;
  hw_uio = new uhal::HwInterface(manager.getDevice ( connectionFileEntryUIO.c_str() ));
  std::cout << "Got the HW interface for UIOuHAL" << std::endl;

  // Get device interface for MemMap 
  std::cout << "Getting HW interface for MemMap" << std::endl;
  std::string connectionFileEntryMemMap("apollo.c2c.vu7p");
  uhal::HwInterface * hw_mmap;
  hw_mmap = new uhal::HwInterface(manager.getDevice ( connectionFileEntryMemMap.c_str() ));
  std::cout << "Got the HW interface for MemMap" << std::endl;

  // Now, read the registers with both interfaces
  std::string registerName(DEFUALT_LOCAL_REGISTER);
  std::cout << "Trying to read register: " << registerName << " with UIOuHAL interface" << std::endl;
  uhal::ValWord<uint32_t> ret_uio;

  // UIOuHAL read 
  ret_uio = hw_uio->getNode(registerName).read();
  hw_uio->dispatch();
  std::cout << "Succesfully read register" << std::endl;
  std::cout << "Value: 0x" << std::hex << ret_uio.value() << std::endl; 
  
  // 'Remote' register using uIOuHAL
  std::string registerNameRemoteUIO(DEFAULT_REMOTE_REGISTER);
  std::cout << "Trying to read register: " << registerNameRemoteUIO << " with UIOuHAL interface" << std::endl;
  try {
    ret_uio = hw_uio->getNode(registerNameRemoteUIO).read();
    hw_uio->dispatch();
    std::cout << "WARNING: NO EXCEPTION THROWN" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << "Exception caught (as expected): " << e.what() << std::endl;
  }

  std::string registerNameMMap(DEFAULT_MMAP_REGISTER);
  std::cout << "Trying to read register: " << registerNameMMap << " with MemMap interface" << std::endl;
  uhal::ValWord<uint32_t> ret_mmap;

  // MemMap read, it should go bad here..
  ret_mmap = hw_mmap->getNode(registerNameMMap).read();
  hw_mmap->dispatch();
  std::cout << "Succesfully read register" << std::endl;
  std::cout << "Value: 0x" << std::hex << ret_mmap.value() << std::endl; 

  return 0;
}

