# This ksh script starts QDB server for SDL on QNX
# Need superuser to start qdb

version=`uname -r`
echo "QNX ${version}"
if [[ ${version} == "6.5.0" ]] then
  LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/qnx650/target/qnx6/x86/usr/lib
  export LD_LIBRARY_PATH
  /usr/sbin/qdb -c policy.ini
elif [[ ${version} == "6.6.0" ]] then
  ln -s -f /lib/libcpp-ne.so.4 /lib/libcpp-ne.so.5
  export TMPDIR=/tmp
  mkdir -p /root/db
  mkdir -p /root/sql
  mkdir -p /root/bkp
  cp policy.sql /root/sql
  cp policy_data.sql /root/sql
  pps
  mkdir -p /pps/qnx/qdb
  qdb
  cp policy /pps/qnx/qdb/config
else
  echo "Unsupported version of QNX"
fi