DB_DIFF_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source $DB_DIFF_PATH"/../shared/include.sh"

#
# Load configuration
#
source $DB_DIFF_PATH"/conf.dist"

if [ -f $DB_DIFF_PATH"/conf" ]; then
	source $DB_DIFF_PATH"/conf"
fi

#
# RUN SCRIPT
#

rm -R $OUTPUT_DIR
mkdir -p $OUTPUT_DIR

op_list=["INSERT","UPDATE","REPLACE","DELETE"]

echo "Comparing databases..."

mysqldbcompare --server1=$DB_1_USER:$DB_1_PASS@$DB_1_HOST --server2=$DB_2_USER:$DB_2_PASS@$DB_2_HOST --difftype=sql $DB_1_NAME:$DB_2_NAME --run-all-tests > "$DB_DIFF_PATH/full.sql"

echo "Splitting files..."

while IFS='' read -r line || [[ -n "$line" ]]
do
	first=${line:0:1}
	if [[ $first == "#" || $first == "" ]];then
	   continue;
	fi

	operation=${line%% *}
	name=${line#* \`azerothcore_clean_world\`.\`}
	name=${name%%\` *}
	if [ ! -z "$name" ]; then
		if [[ "${op_list[@]}" =~ "${operation}" && ! -z "$operation" ]]; then
			echo $line >> $OUTPUT_DIR$name"___"${operation,,}".sql"
		#else
			#echo $line >> $OUTPUT_DIR"AAA-base.sql"
		fi
	fi
done < "full.sql"

echo "Removing full.sql..."
rm "$DB_DIFF_PATH/full.sql"

echo "Sorting process..."

for f in $OUTPUT_DIR*
do
	sort -o $f $f
done

