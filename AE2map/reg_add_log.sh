# This script file imports the .log entry in the registry

source reg_info.sh
source reg_info_log.sh

wine $REG IMPORT $logRegFile
