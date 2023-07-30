cd /proc
for i in `echo [0-9]*`
do
	echo -n -e Checking $i '\t\t'
	# /home/krish/Projects/parse_raddr/a.out $i
	/home/krish/Projects/parse_raddr/a.out $i > /tmp/out 2>&1
	if [ $? -eq 0 ]
	then
		echo ""
	else
		cat /tmp/out
	fi
done
