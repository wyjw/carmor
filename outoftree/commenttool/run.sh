# The comment tool is built in the build_dir. Anno is the annotation needed for the partitioning
# The annotation file should be one line | (func_name) (machine_num) |, where (machine_num) is the machine
# where the file is run.
./build/comment-tool ../test_cases/split2.cc -anno ../test_cases/split2.info
