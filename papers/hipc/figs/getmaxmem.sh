
for i in $(ls -1 $1/*dot*); do 
  echo -n "$i : ";
  MAXSIZE=$(cat $i | awk '{print  $2;}'| sort -n |tail -n 1)
  echo "$MAXSIZE";
  MIB=$(echo "$MAXSIZE/1024/1024"|bc -l)
done
