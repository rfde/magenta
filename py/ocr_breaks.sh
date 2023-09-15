for i in breaks*png; do convert -flip $i $i.mirror.png; done
for i in breaks_*mirror.png; do tesseract $i $i.txt -c "tessedit_char_whitelist=lg{}0123456789abcdef"; done
for i in *.txt.txt; do echo "\n\n#####";echo $i; cat $i; done | grep -B 1 "flag{0123456789abcdef0123456789abcdef0123456789abcdef}"  