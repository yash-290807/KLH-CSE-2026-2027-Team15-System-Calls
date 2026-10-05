#!/bin/bash

echo "=========================================="
echo " SKILL 5: SINGLE & DOUBLE QUOTES IN BASH"
echo "=========================================="

echo
echo "1. SINGLE QUOTES - LITERAL CONTENT"
echo "-----------------------------------"

name="Bharath"

single='Hello $name'
echo "Single quoted string:"
echo "$single"

echo
echo "Result: \$name is NOT expanded inside single quotes."


echo
echo "2. DOUBLE QUOTES - VARIABLE EXPANSION"
echo "--------------------------------------"

double="Hello $name"
echo "Double quoted string:"
echo "$double"

echo
echo "Result: \$name IS expanded inside double quotes."


echo
echo "3. PRESERVING SPACES"
echo "--------------------"

message="This is a message with multiple words"

echo "Message:"
echo "$message"

echo
echo "Without quotes, Bash would split words into separate arguments."
echo "Double quotes preserve the complete string."


echo
echo "4. SINGLE QUOTES WITH SPECIAL CHARACTERS"
echo "----------------------------------------"

literal='The value is $100 and the command is `date`'

echo "Literal content:"
echo "$literal"

echo
echo "The \$ and backticks remain literal."


echo
echo "5. DOUBLE QUOTES WITH VARIABLE EXPANSION"
echo "----------------------------------------"

city="Hyderabad"
country="India"

location="I live in $city, $country"

echo "$location"


echo
echo "6. NESTED QUOTES"
echo "----------------"

nested="He said 'Hello' to me"

echo "$nested"

single_nested='He said "Hello" to me'

echo "$single_nested"


echo
echo "7. QUOTED COMMAND OUTPUT"
echo "------------------------"

today=$(date)

echo "Today is: $today"


echo
echo "8. COMMAND AS A VARIABLE"
echo "------------------------"

command_name="date"

echo "Command stored in variable: $command_name"

echo
echo "Executing command:"
$command_name


echo
echo "9. ARRAY / WORD SPLITTING TEST"
echo "------------------------------"

text="one two three"

echo "Original text:"
echo "$text"

echo
echo "Using quoted variable:"
printf '<%s>\n' "$text"

echo
echo "Using unquoted variable:"
printf '<%s>\n' $text


echo
echo "10. SPECIAL EDGE CASE"
echo "---------------------"

special='Hello $USER $(date) `date` "quotes"'

echo "Single quotes preserve everything:"
echo "$special"


echo
echo "11. DOUBLE QUOTE EDGE CASE"
echo "--------------------------"

special_double="Hello $USER"

echo "Double quotes expand USER:"
echo "$special_double"


echo
echo "12. EMPTY QUOTED STRINGS"
echo "------------------------"

empty=''

echo "Length of empty string: ${#empty}"


echo
echo "13. STRING COMPARISON"
echo "---------------------"

value="hello"

if [ "$value" = "hello" ]; then
    echo "PASS: Strings are equal."
else
    echo "FAIL: Strings are different."
fi


echo
echo "14. PARSING VALIDATION"
echo "----------------------"

if bash -n skill5.sh; then
    echo "PASS: Bash syntax is valid."
else
    echo "FAIL: Bash syntax error."
fi


echo
echo "15. QUOTED COMMAND WITH SPACES"
echo "------------------------------"

filename="my test file.txt"

echo "Creating file:"
echo "$filename"

touch "$filename"

if [ -f "$filename" ]; then
    echo "PASS: File created successfully."
else
    echo "FAIL: File was not created."
fi


echo
echo "16. VERIFY FILE"
echo "---------------"

ls -l "$filename"


echo
echo "17. CLEANUP TEST FILE"
echo "---------------------"

rm "$filename"

if [ ! -f "$filename" ]; then
    echo "PASS: Test file removed."
else
    echo "FAIL: Test file still exists."
fi


echo
echo "=========================================="
echo " ALL SKILL 5 TESTS COMPLETED"
echo "=========================================="
