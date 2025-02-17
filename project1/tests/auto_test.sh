#!/bin/bash

touch ~/Operating-Systems/project1/output/awyatt_test_out.txt
touch ~/Operating-Systems/project1/output/thanhd_test_out.txt
touch ~/Operating-Systems/project1/output/gscott_test_out.txt
touch ~/Operating-Systems/project1/output/esc_test_out.txt
touch ~/Operating-Systems/project1/output/rstiles1_test_out.txt
touch ~/Operating-Systems/project1/output/rstiles2_test_out.txt

#Run tests and output them to the output folder
#Test 1
/projects/1/project1 ~/Operating-Systems/project1/tests/awyatt.txt > ~/Operating-Systems/project1/output/awyatt_test_out.txt
find . ! -name 'auto_test.sh' -type f -exec rm -f {} +
sudo rm -R -- */

#Test 2
/projects/1/project1 ~/Operating-Systems/project1/tests/thanhd.txt > ~/Operating-Systems/project1/output/thanhd_test_out.txt
find . ! -name 'auto_test.sh' -type f -exec rm -f {} +
sudo rm -R -- */

#Test 3
/projects/1/project1 ~/Operating-Systems/project1/tests/gscott-1.txt > ~/Operating-Systems/project1/output/gscott_test_out.txt
find . ! -name 'auto_test.sh' -type f -exec rm -f {} +
sudo rm -R -- */

#Test 4
/projects/1/project1 ~/Operating-Systems/project1/tests/esc.txt > ~/Operating-Systems/project1/output/esc_test_out.txt
find . ! -name 'auto_test.sh' -type f -exec rm -f {} +
sudo rm -R -- */

#Test 5
/projects/1/project1 ~/Operating-Systems/project1/tests/rstiles1.txt > ~/Operating-Systems/project1/output/rstiles1_test_out.txt
find . ! -name 'auto_test.sh' -type f -exec rm -f {} +
sudo rm -R -- */

#Test 6
/projects/1/project1 ~/Operating-Systems/project1/tests/rstiles2.txt > ~/Operating-Systems/project1/output/rstiles2_test_out.txt
find . ! -name 'auto_test.sh' -type f -exec rm -f {} +
sudo rm -R -- */

#diff files
echo "Test 1";
diff -b -B ~/Operating-Systems/project1/output/awyatt_test_out.txt ~/Operating-Systems/project1/answers/awyatt.out

echo "Test 2";
diff -b -B ~/Operating-Systems/project1/output/thanhd_test_out.txt ~/Operating-Systems/project1/answers/thanhd.out

echo "Test 3";
diff -b -B ~/Operating-Systems/project1/output/gscott_test_out.txt ~/Operating-Systems/project1/answers/gscott-1.out

echo "Test 4";
diff -b -B ~/Operating-Systems/project1/output/esc_test_out.txt ~/Operating-Systems/project1/answers/esc.txt.out

echo "Test 5";
diff -b -B ~/Operating-Systems/project1/output/rstiles1_test_out.txt ~/Operating-Systems/project1/answers/rstiles1.out

echo "Test 6";
diff -b -B ~/Operating-Systems/project1/output/rstiles2_test_out.txt ~/Operating-Systems/project1/answers/rstiles2.out