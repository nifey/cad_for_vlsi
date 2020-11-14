if [ $# -ne 2 ];
then
	echo "Usage: bash analyse.sh result_file num_stages"
	exit
fi

echo "Running correctness check"
echo -n "Number of errors: "
cat $1 | awk '{
	if (NR%2==1) {print $2" "$4" "$6;}
	}' | awk -v num_stages=$2 '{
	a[NR] = $1; b[NR] = $2; if (NR>(num_stages+1)) {print "0x"a[NR-(num_stages+1)]" 0x"b[NR-(num_stages+1)]" 0x"$3;}
	}' | awk -M --non-decimal-data '{
		if ($1 * $2 != $3) {
			print ("Error " $1 " * " $2 " = "$3);
		}
	}' | wc -l
