if [ $# -ne 1 ];
then
	echo "Usage: bash analyse.sh result_file"
	exit
fi

echo "Running correctness check"
cat $1 |\
	egrep "^[ \t]*[0-9]+0000" -B1 |\
	sed '
	/^[ \t]*[0-9]\+0000/d;
	/^--$/d;
	/^[ \t]*0/d;
	s/^[ \t]*[0-9]\+ \([0-9abcdef]*\) + \([0-9abcedf]*\) = \([0-9abcdef]*\)/0x\1,0x\2,0x\3/' |\
	awk -F, --non-decimal-data '{
		if ($1 + $2 != $3) {
			print ("Error " $1 " + " $2 " = "$3);
		}
	}' |\

echo "Getting latency information"
cat $1 |\
	egrep "^[ \t]*[0-9]+0000" -B1 |\
	sed '
	/^[ \t]*[0-9]\+0000/d;
	/^--$/d;
	s/^[ \t]*\([0-9]\+\)/\1/' |\
	awk '{sum += $1%10000} END {print sum/NR}'
