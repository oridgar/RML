#!/bin/bash
source ./cygwin_vars.sh
#lin_dir=
#lin_user=
#lin_addr=
/cygdrive/c/Program\ Files\ \(x86\)/DOSBox-0.74/DOSBox.exe -c "..\compile.bat"

#RESULT.TXT problems format

echo "--------"
echo "Warnings"
echo "--------"
echo "Type Location\file Line Message"
echo "-------------------------------"
cat ./obj/RESULT.TXT | grep -v "Warning messages:" | grep "Warning"
warnings=`cat ./obj/RESULT.TXT | grep -v "Warning messages:" | grep "Warning" | wc -l`
if [[ $warnings -eq 0 ]]; then 
	echo "no warnings";
else 
	echo "total: $warnings"; 
fi;
echo
echo "------"
echo "Errors"
echo "------"
echo "Type Location\file Line Message"
echo "-------------------------------"
cat ./obj/RESULT.TXT | grep -v "Error messages:    None" | grep "Error"
errors=`cat ./obj/RESULT.TXT | grep -v "Error messages:    None" | grep "Error" | wc -l`
if [[ $errors -eq 0 ]]; then 
	echo "no errors"; 
	echo
	if [[ `uname -o` == "Cygwin" ]]; then
		ssh $lin_user@$lin_addr "cd $lin_dir; make"
		if [[ `ps | grep -i bochs | wc -l` -eq 0 ]]; then
			#/cygdrive/c/Program\ Files\ \(x86\)/Bochs-2.6.8/bochsdbg.exe -f bochsrc2.bxrc -q
			/cygdrive/c/Program\ Files\ \(x86\)/Bochs-*/bochs.exe -f bochsrc2.bxrc -q
		fi
	fi
else 
	echo "total: $errors"; 
	echo "FAILED!"
	read -p "Press any key to continue..."
fi;
