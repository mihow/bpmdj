#!/bin/bash
echo "#!/bin/bash" >t
find $1 -name "*.mp3" |
sed 's/music\/cd/.\/cbpm-count -v \"music\/cd/' |
sed 's/.mp3/.mp3\"/' >>t

find $1 -name "*.MP3" |
sed 's/music\/cd/.\/cbpm-count -v \"music\/cd/' |
sed 's/.MP3/.MP3\"/' >>t

cat t
chmod 755 t
