# log4cxx_mutex_error
using Ubuntu 18.04  
To reproduce first install dependecies for log4cxx dev  
```sudo apt-get install build-essential libapr1-dev libaprutil1-dev gzip zip```  
compile and install logging-log4cxx  
using ncat so need to install namp  
```sudo apt install nmap```  

compile this test app  
```mkidr build```  
```cd build```  
```cmake ..```  
```make```  

first run in a differnt terminal  
```ncat  -v -k -l 4445 ```  

then run this app  
```./test```  

kill the ncat process  

should get exception

log4cxx: IO Exception : status code = 104  
failed:Resource deadlock avoided  
