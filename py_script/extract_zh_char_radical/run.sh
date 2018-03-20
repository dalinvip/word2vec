nohup python -u extract_zh_char_radical.py --input ./Data/giga_small.txt --output ./Data/giga_small_out.txt --save_radical ./Data/radical > log_extract_radical 2>&1 &
tail -f log_extract_radical
