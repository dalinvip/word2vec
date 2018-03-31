
cp -rf ./bin/word2vec .

path_input=./sample
path_out=./sample
path_inradical=./sample
path_incomponent=./sample

nohup ./word2vec skipgram -input ${path_input}/giga_char_sample.txt -output ${path_out}/giga_skipgram.300d -lr 0.025 -dim 300 -ws 5 -epoch 10 -minCount 10 -neg 5 -loss ns -thread 8 -t 1e-4 -lrUpdateRate 100 > log_skipgram_300d 2>&1 &

nohup ./word2vec subradical -input ${path_input}/giga_char_sample.txt -inradical ${path_inradical}/char_radical_sample.txt -output ${path_out}/giga_subradical.300d -lr 0.025 -dim 300 -ws 5 -epoch 10 -minCount 10 -neg 5 -loss ns -thread 8 -t 1e-4 -lrUpdateRate 100 > log_subradical_300d 2>&1 &

nohup ./word2vec subcomponent -input ${path_input}/giga_char_sample.txt -incomponent ${path_incomponent}/char_component_sample.txt -output ${path_out}/giga_subcomponent.300d -lr 0.025 -dim 300 -ws 5 -epoch 10 -minCount 10 -neg 5 -loss ns -thread 8 -t 1e-4 -lrUpdateRate 100 > log_subcomponent_300d 2>&1 &
