#!/bin/bash
## OMV-Support script

# R. Lindlein aka Solo0815 in the OMV-Forums

# Bugs fixed / features added:
# - upload to sorunge.us
# - AddIn-system -> see sample-addin in /etc/omv-support.d
# - basic info with smartctl
# - detecting the systemdrive from fstab together with /dev/disk/by-uuid

# ToDo.
# - add js files -> ls -la/var/www/openmediavault/js/omv/module/admin/
# - check for needed packages -> smartctl, curl, lshw(?)

###########################################################################################
### Variables

VERSION="0.2.0"
SCRIPTDATE="$(date +%y%m%d-%H%M%S)"
TITLE="OMV-Supportscript v. $VERSION"
BACKTITLE="OpenMediaVault Support"
OMVAPT="/etc/apt/sources.list.d"
TMPFOLDER="/tmp/omv-support"
SUPPORTFILE_DIR="/etc/omv-support.d"
# Detecting Systemdrive (/dev/sdX and UUID). It will be used later
SYSDRIVE_UUID="$(cat /etc/fstab | egrep "UUID.* / " | awk '{print $1}' | sed 's/UUID=//g')"
SYSDRIVE="$(ls -l /dev/disk/by-uuid/ | grep $SYSDRIVE_UUID | awk '{print $11}' | sed 's/..\/..\///g')"
DEBUG=false

# echo "SYSDRIVE_UUID: $SYSDRIVE_UUID"
# echo "SYSDRIVE: /dev/$SYSDRIVE"

# import OMV-variables
. /etc/default/openmediavault
# enable this later!!!
# if ! $DEBUG; then
# 	if [ -f /etc/default/openmediavault ]; then
# 		. /etc/default/openmediavault
# 	else
# 		echo "OpenMediaVault seems not to be installed"
# 		echo "No defaults for OpenMediaVault found in /etc/default/openmediavault"
# 		echo "exiting ..."
# 		echo
# 		exit 1
# 	fi
# fi
# Sample - possible usefull variables from /etc/default/openmediavault
# OMV_DPKGARCHIVE_DIR="/var/cache/openmediavault/archives"
# OMV_DOCUMENTROOT_DIR="/var/www/openmediavault"
# OMV_CACHE_DIR="/var/cache/openmediavault"

# import the needed Libs
. /usr/local/bin/omv-support-libs

###########################################################################################
### Functions

f_aborted() {
	whiptail --title "Script cancelled" --backtitle "${BACKTITLE}" --msgbox "         Aborted!" 7 32
	exit 0
}

# Upgrade to latest version, not functional atm
# f_update-to-latest() {
#     apt-get -y clean
#     apt-get -y update
# 
#     apt-get -y upgrade
#     apt-get -y dist-upgrade
# }

f_automatic() {
	SELECTION="00-basic-info 01-sources 02-packages 03-filesystem 04-raid 06-samba 99-other"  # only the default-scripts are executed, more could be added later
	# 05-free_space --> left out of the list, takes too long to finish

	for I in $SELECTION; do
		#eval $(\. $(echo ${I%%-*} | sed 's/"//g')-* $(echo ${I%%-*} | sed 's/"//g')-*) # executes the AddIn and gives the filename as a variable -> basename $0 is not working in the AddIn-files
		. "$SUPPORTFILE_DIR/$I" "$I"
	done

	# output to file
	#cat $TMPFOLDER/* > $TMPFOLDER/ready_to_upload

	# Hi ryecoaaron,
	# don't know if you need the code below. The script should automatically output info to stdout
	# enable it maybe for testing
	
	# output to stdout (second time). The first time is in the f_log-function
	#cat $TMPFOLDER/*
}


###########################################################################################
### Begin of script

# if executed by the webgui
if [ "$1" = "webgui" ]; then
	f_log "$(basename $0) executed by WebGUI-plugin"
	f_automatic
	exit 0
fi

# check available diskspace on systemdrive
if $DEBUG; then echo "SYSDRIVE: $SYSDRIVE"; fi
SYSDRIVE_AVAILABLE=$(df -B KB /dev/$SYSDRIVE | grep / | awk '{print $4}' | sed 's/kB//g')

#echo "SYSDRIVE_AVAILABLE: $SYSDRIVE_AVAILABLE"
if [ $SYSDRIVE_AVAILABLE -lt 1024 ]; then 
	whiptail --title "Script cancelled" --backtitle "${BACKTITLE}" --msgbox "The free space on your Systemdrive /dev/$SYSDRIVE is less than 1 MB" 11 38
	f_aborted
fi

# check for curl - it's needed for the upload-function to sprunge.us
UPLOAD="true" # we guess curl is installed
if ! dpkg -l | grep " curl " >/dev/null 2>&1; then 
	whiptail --title "'curl' not found!" --backtitle "${BACKTITLE}" --yesno "'curl' is not installed on your system. If you want to upload your Info to sprunge.us then 'curl is required. Do you want to install it?" 11 38
	if [ $? = 1 ]; then 
		whiptail --title "Upload deactivated" --backtitle "${BACKTITLE}" --msgbox "The auto-uploading function is disabled. You'll have to copy the Info manually in the corresponding thread." 11 38
		UPLOAD="false" # curl not installed, so upload disabled
	else
		apt-get install -y curl 
	fi
fi

# make sure no old files are there -> delete the $TMPFOLDER
if [ -d $TMPFOLDER ]; then
	rm -Rf $TMPFOLDER
fi
# create $TMPFOLDER again
mkdir $TMPFOLDER

whiptail --title "${TITLE}" --backtitle "${BACKTITLE}" --yesno --defaultno "This script will help to collect some Information about your OMV-System to help the supporters\n\nPlease read the following instructions carefully!\nTo collect the Info, the following steps are required:\n1. select the Infos the script should collect\n2. have a look at the collected infos\n3. optionally you can upload the infos to sprunge.us\n\n\nContinue?" 20 78

if [ $? = 1 ]; then f_aborted; fi

### Dynamic Menu:
# first read all the AddIns and remove the "/etc/omv-support.d"
ADDIN_FILES="$(for ADDINS in $SUPPORTFILE_DIR/[0-9][0-9]-*; do echo -n "$ADDINS" | sed 's/\/etc\/omv-support\.d//g' ; done)"

# To create dynamic menu, you have to change the IFS
# See: http://stackoverflow.com/questions/14191797/inserting-code-from-a-string-or-array-in-to-whiptail
# There's maybe another (better) solution, but I haven't found one. Please let me know, if YOU know one :)
oIFS="$IFS"
IFS="/"
# create the menu-entries
# sample: 73/some description/OFF/
#
# 1. echo $ADDIN_FILES and remove the first slash "/"
# 2. remove all behing the two digits to create the number (##)
# 3. grep the DESCRIPTION from the file and add it
# 4. add /OFF so the menuentry for whiptail is complete
MENUENTRIES="$(for MENUPOINTS in $(echo "$ADDIN_FILES" | sed 's/^\///g'); do \
if [ $MENUPOINTS = "00-basic-info" ]; then continue; fi; \
echo -ne "${MENUPOINTS%%-*}/$(grep DESCRIPTION "$SUPPORTFILE_DIR/$MENUPOINTS" | sed 's/DESCRIPTION="\(.*\)"/\1/g')/OFF/"; done)"

## Checklist
RESULT="$(whiptail --title "${TITLE}" --backtitle "${BACKTITLE}" --checklist "Choose the infos to collect with SPACEBAR:" 26 58 10 $MENUENTRIES 3>&1 1>&2 2>&3)"

# Cancelling the selection gives empty $RESULT
if [ -z "$RESULT" ]; then
	f_aborted
fi

## ^^ the 3>&1 1>&2 2>&3 thing is from here: 
# http://stackoverflow.com/questions/1970180/whiptail-how-to-redirect-output-to-environment-variable
IFS=$oIFS # switch it back to normal

# ## basic info - collected in every run
# . /etc/omv-support.d/00-basic-info 00-basic-info

# add 00 to the selection
RESULT="$(echo $RESULT | sed 's/^/"00" /g')"
echo "RESULT: $RESULT"
# execution of the selected scripts
for I in $RESULT; do # gives the selected number, e.g. 04
	echo "for schleife 1"
	# gives back all files in $SUPPORTFILE_DIR with the number in front, e.g. 04-raid
	for J in $SUPPORTFILE_DIR/$(echo ${I} | sed 's/"//g')-*; do
		echo "for schleife 2"
		echo "J: $J --- ${J##*/}"
		. $J $(echo ${J##*/}) # executes the AddIn and gives the filename as a variable -> basename $0 is not working in the AddIn-files
	done
	
done

# the user can check the collected info
whiptail --title "All info collected" --backtitle "${BACKTITLE}" --yesno "All of the selected info is collected. In the next step you can, if it should be uploaded. Do you want to view it before uploading? " 11 38
if [ $? = 0 ]; then
	# view every file in $TMPFOLDER
	for SUPPORTFILES in $TMPFOLDER/*; do
		# view it in CLI
		#cat $SUPPORTFILES
		# view it in whiptail
		whiptail --clear --textbox --title "$SUPPORTFILES" --scrolltext "$SUPPORTFILES" 32 78
	done
fi

# uploading function to sprunge.us
# example: http://www.commandlinefu.com/commands/matching/pastebin/cGFzdGViaW4=/sort-by-votes
if [ "$UPLOAD" = "true" ]; then
	whiptail --title "Upload" --backtitle "${BACKTITLE}" --yesno "Your info is ready to upload to sprunge.us. Do you want to auto-upluad it now?" 11 38
	if [ $? = 0 ]; then
		# create a whole doc
		cat $TMPFOLDER/* > $TMPFOLDER/ready_to_upload
		RESULT="$(cat $TMPFOLDER/ready_to_upload | curl -s -F 'sprunge=<-' http://sprunge.us)"
		sleep 1
		echo
		echo "Link to view your results online: $RESULT"
		whiptail --title "Upload successful" --backtitle "${BACKTITLE}" --msgbox "Your Upload was successful. Just copy this Link in the corresponding forum thread:\n\n$RESULT\n\nYou find the collected info in $TMPFOLDER" 11 44
	else
		whiptail --title "No Upload" --backtitle "${BACKTITLE}" --msgbox "You selected not to upload your info. Please copy and paste the info in $TMPFOLDER to the corresponding forum thread.\nThank you" 11 44
	fi
else
	whiptail --title "No Upload" --backtitle "${BACKTITLE}" --msgbox "The Upload function was disabled. Please copy and paste the data in $TMPFOLDER to the corresponding forum thread.\nThank you" 11 44
fi
exit 0 
