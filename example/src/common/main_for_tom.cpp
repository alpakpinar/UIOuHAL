#include <iostream>
#include <memory>

#include <boost/program_options.hpp>

#include "uhal/uhal.hpp"

// Default XML connection file to be used
#define DEFAULT_CONNECTION_FILE "file://../address_table/connections_for_tom.xml"

// Default register names to read
#define DEFUALT_LOCAL_REGISTER  "PL_MEM.ARM.CPU_LOAD"
#define DEFAULT_REMOTE_REGISTER "CM_V_INFO.GIT_HASH_1" 
#define DEFAULT_MMAP_REGISTER   "info.magic"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  using namespace std;

  string connectionFile = DEFAULT_CONNECTION_FILE;
  string localRegister  = DEFUALT_LOCAL_REGISTER;
  string remoteRegister = DEFAULT_REMOTE_REGISTER;
  string mmapRegister   = DEFAULT_MMAP_REGISTER;

  /*
   * Set up CLI program options.
   */
  po::options_description cli_options("CLI options");
  cli_options.add_options()
    ("help",  "Help screen")
    ("cfile"      , po::value<string>(), "XML connection file to use")
    ("lRegUIO"    , po::value<string>(), "Name of the local UIO register")
    ("rRegUIO"    , po::value<string>(), "Name of the remote UIO register")
    ("regMmap"    , po::value<string>(), "Name of the MemMap register")
  ;

  po::variables_map vmap;        
  po::store(po::parse_command_line(argc, argv, cli_options), vmap);
  po::notify(vmap);

  // Display help message and exit.
  if (vmap.count("help")) {
    cout << cli_options << endl;
    return 0;
  }

  // If CLI options were given, override the defaults set earlier
  if (vmap.count("cfile")) {
    connectionFile = vmap["cfile"].as<string>();
  }
  if (vmap.count("lRegUIO")) {
    localRegister = vmap["lRegUIO"].as<string>();
  }
  if (vmap.count("rRegUIO")) {
    remoteRegister = vmap["rRegUIO"].as<string>();
  }
  if (vmap.count("regMmap")) {
    mmapRegister = vmap["regMmap"].as<string>();
  }

  /*
   * Launch example program!
   */

  // Connection file to use: Path is relative to the bin/main executable
  // This connection file has two set of hardware interfaces:
  // 1. UIOuHAL interface, 2. MemMap interface
  cout << "Using connection file: " << connectionFile << endl;

  // Get the connection manager
  uhal::setLogLevelTo(uhal::Notice());
  uhal::ConnectionManager manager( connectionFile.c_str(), vector<string>(1,"uioaxi-1.0") ); 
  cout << "Constructed ConnectionManager" << endl;

  // Get device interface for UIOuHAL 
  cout << "Getting HW interface for UIOuHAL" << endl;
  string connectionFileEntryUIO("test.0");
  uhal::HwInterface * hw_uio;
  hw_uio = new uhal::HwInterface(manager.getDevice ( connectionFileEntryUIO.c_str() ));
  cout << "Got the HW interface for UIOuHAL" << endl;

  // Get device interface for MemMap 
  cout << "Getting HW interface for MemMap" << endl;
  string connectionFileEntryMemMap("apollo.c2c.vu7p");
  uhal::HwInterface * hw_mmap;
  hw_mmap = new uhal::HwInterface(manager.getDevice ( connectionFileEntryMemMap.c_str() ));
  cout << "Got the HW interface for MemMap" << endl;

  // Now, read the registers with both interfaces
  cout << "Trying to read register: " << localRegister << " with UIOuHAL interface" << endl;
  uhal::ValWord<uint32_t> ret_uio;

  // UIOuHAL read 
  ret_uio = hw_uio->getNode(localRegister).read();
  hw_uio->dispatch();
  cout << "Succesfully read register" << endl;
  cout << "Value: 0x" << hex << ret_uio.value() << endl; 
  
  // 'Remote' register using uIOuHAL
  cout << "Trying to read register: " << remoteRegister << " with UIOuHAL interface" << endl;
  try {
    ret_uio = hw_uio->getNode(remoteRegister).read();
    hw_uio->dispatch();
    cout << "WARNING: NO EXCEPTION THROWN" << endl;
  }
  catch (const exception& e) {
    cout << "Exception caught (as expected): " << e.what() << endl;
  }

  // Do the MemMap read
  cout << "Trying to read register: " << mmapRegister << " with MemMap interface" << endl;
  uhal::ValWord<uint32_t> ret_mmap;

  // MemMap read, it should go bad here..
  ret_mmap = hw_mmap->getNode(mmapRegister).read();
  hw_mmap->dispatch();
  cout << "Succesfully read register" << endl;
  cout << "Value: 0x" << hex << ret_mmap.value() << endl; 

  return 0;
}

