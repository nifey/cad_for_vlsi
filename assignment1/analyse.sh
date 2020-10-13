if [ $# -ne 1 ];
then
	echo "Usage: bash analyse.sh result_file"
	exit
fi

echo "Running correctness check"
echo -n "Number of errors: "
cat $1 |\
	egrep "^[ \t]*[0-9]+0000000" -B1 |\
	sed '
	/^[ \t]*[0-9]\+0000000/d;
	/^--$/d;
	/^[ \t]*0/d;
	s/^[ \t]*[0-9]\+ \([0-9abcdef]*\) + \([0-9abcedf]*\) = \([0-9abcdef]*\)/0x\1,0x\2,0x\3/' |\
	awk -F, -M --non-decimal-data '{
		if ($1 + $2 != $3) {
			print ("Error " $1 " + " $2 " = "$3);
		}
	}' | wc -l

echo "Getting latency information"
cat $1 |\
	egrep "^[ \t]*[0-9]+0000000" -B1 |\
	sed '
	/^[ \t]*[0-9]\+0000000/d;
	/^--$/d;
	/^[ \t]*0/d;
	s/^[ \t]*\([0-9]\+\)/\1/' |\
	awk '
	BEGIN {
		min = 99999;
		max = 0;
	}
	{
		if ($1%10000000 < min) {
			min = $1%10000000;
		}
		if ($1%10000000 > max) {
			max = $1%10000000;
		}
		sum += $1%10000000
	}
	END {
		printf("Min latency: %d\n", min);
		printf("Max latency: %d\n", max);
		printf("Avg latency: %f\n", sum/NR);
	}'
