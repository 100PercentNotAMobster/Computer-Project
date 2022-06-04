#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

#define FILE_SIZE 200000
#define RAM_SIZE 100000
//maybe change to 16 bit instruction widht? even 32? idk
//I need to learn C better :( I dont think I know enough ::::(((((((
char PC[16]; //program counter
char reg1[16]; //this register will be used for memory (will read it to get pointer of where to go, for example during jmp)
char reg2[16];
char flags[5];
char temp[FILE_SIZE][9]; //this is used when getFromMemory() is called
/*
flag 1: overflow
flag 2: equal
flag 3: greater
flag 4: less
flag 5: memory error (tried to read memory that does not exist)
*/
char RAM[RAM_SIZE][8];

void getReg1(char *output){
    for(int i = 0; i < 16; i++){
        *output = reg1[i];
        output++;
    }
}

void getReg2(char *output){
    for(int i = 0; i < 16; i++){
        *output = reg2[i];
        output++;
    }
}

void getPC(char *output){
    for(int i = 0; i < 16; i++){
        *output = PC[i];
        output++;
    }
}


void getFromMemory(int lineStartAt, int linesToGet, char *output){
    //gets a specified number of "bytes" from memory, starting at lineStartAt. Stores the result in output
    //stores its result in the temp variable in the global variables section, temporarily.
    //starts at zero
    Sleep(2); //always use Sleep with a captial "s", otherwise it waits ten actual secondss instead of ten ms
    //does not null terminate
    FILE *fh;
    if(lineStartAt >= FILE_SIZE * 8){
        flags[4] = '1'; //sets the memory error flag
        return;
    }

    int incarmentBy = 0;
    fh = fopen("Main Memory.txt", "r");
    char useless[8];
    for(int i = 0; i < lineStartAt; i++){
        incarmentBy += 8;
    }
    fseek(fh, incarmentBy, SEEK_SET);
    for(int i = 0; i < linesToGet; i++){
            fgets(temp[i], 9, fh);
            incarmentBy += 8;
            fseek(fh, incarmentBy, SEEK_SET);
    }
    int index = 0;
    while(1){

        char firstChar = temp[index][0];
        if(firstChar == '\0'){
            break;
        }

        else{
            for(int i = 0; i < 8; i++){
                output[(index * 8) + i] = temp[index][i];
            }
        }
        index++;
    }

    fclose(fh);
}

int binaryToIntUnsignedFive(char binary[9]){
    //takes an 8 bit binary number and returns the integer value of the leftmost five bits (really these are char bytes)
    int returnNum = 0;
    int index = 5;
    for(int i = 0; i <= 5; i++){
        if(binary[i] == '1'){
            returnNum += twoToThePowerOf(index-1);
        }
        else{
        }
        index--;

    }
    return returnNum;

}

void substr(char original[8], char newstr[3]){
    //creates a substring out of the last 3 characters in the original string. does not null terminate
    for(int i = 5; i < 8; i++){
        newstr[i-5]= original[i];
    }
}

void substrMov(char original[8], char newstr[2]){
    //creates a substring out of the third last and second last characters of a string. does not null terminate
    for(int i = 5; i < 7; i++){
        newstr[i-5]= original[i];
    }
}

int getOperand(char *opcode){
    //gets the operand in decimal int form from a string of 1's and 0's
    //the last 3 bits are for the operand
    char operand[3];
    substr(opcode, operand);
    return binaryToIntUnsigned(operand, 3);
}

int getOperandMov(char *opcode){
    //gets the operand in decimal int form from a string of 1's and 0's
    //this is for the mov instructions operanads
    //the third last and second last bits are used for the operand
    char operand[2];
    substrMov(opcode, operand);
    return binaryToIntUnsigned(operand, 2);
}

void *substrAny(char *original, char *newChar, int number, int startAt){
    //copys a string into a smaller string of any size
    for(int i = startAt; i < number + startAt; i++){
        newChar[i-startAt] = original[i];
    }
}

void setReg1(char *setTo, int length){
    for(int i = 0; i < length; i++){
        reg1[i] = setTo[i];
    }
}

void setReg2(char *setTo, int length){
    for(int i = 0; i < length; i++){
        reg2[i] = setTo[i];
    }
}

void doOp(char *opcode){
    //does an op code specified by the string of 0's and 1's opcode paramater
    //takes 17 bit instructions (I know it's weird but whatever, it's technically possible)
        int intOpcode = binaryToIntUnsignedFive(opcode);
        if(intOpcode == 0){
            //does the nop op
            nop();
        }
        if(intOpcode == 1){
            //does the mov op
            printf("calling mov\n");
            mov(opcode);
        }
        if(intOpcode == 2){
            die();
        }
        if(intOpcode == 3){
            cmp(opcode);
        }
        if(intOpcode == 4){
            not(opcode);
        }
        if(intOpcode == 5){
            and(opcode);
        }
        if(intOpcode == 6){
            or(opcode);
        }
        if(intOpcode == 7){
            xor(opcode);
        }
        if(intOpcode == 8){
            jmp(opcode);
        }

}

void mov(char *opcode){
    printf("in mov\n");
    char smallStr[5];
    substrAny(opcode, smallStr, 5, 11);
    int operand = binaryToIntUnsigned(smallStr, 5);
    printf("operand: %d\n", operand);

    if(operand == 0){
       setReg2(reg1, 16);
    }
    else if(operand == 1){
        setReg1(reg2, 16);
    }
    else if(operand == 2){
        setPC(reg1);
    }
    else if(operand == 3){
        setPC(reg2);

    }
    else if(operand == 4){
        //gets the content at the memory address in register 1 and puts it in register 1
        char toPut[16];
        char toStartAtBin[16];
        getReg1(&toStartAtBin);
        int num = binaryToIntUnsigned(toStartAtBin, 16);
        char *output;
        getFromRAM(num, 2, output);
        setReg1(output, 16);
    }
    else if(operand == 5){
        //gets the content at the memory address in register 2 and puts it in register 2
        char toPut[16];
        char toStartAtBin[16];
        getReg2(&toStartAtBin);
        int num = binaryToIntUnsigned(toStartAtBin, 16);
        char *output;
        getFromRAM(num, 2, output);
        setReg2(output, 16);

    }
    else if(operand == 6){
        //gets the content at the memory address in register 1 and puts it in register 2
        char toPut[16];
        char toStartAtBin[16];
        getReg1(&toStartAtBin);
        int num = binaryToIntUnsigned(toStartAtBin, 16);
        char output[16];
        getFromRAM(num, 2, output);
        setReg2(output, 16);

    }
    else if(operand == 7){
        //gets the content at the memory address in register 2 and puts it in register 1
        char toPut[16];
        char toStartAtBin[16];
        getReg2(&toStartAtBin);
        int num = binaryToIntUnsigned(toStartAtBin, 16);
        char output[16];
        getFromRAM(num, 2, output);
        setReg1(output, 16);
    }

    else if(operand == 8){
        //go to the pointer pointed to by register 1 and write what's in register 2 to that address
        char toPut[16];
        char toFind[16];
        getReg1(toFind);
        int num = binaryToIntUnsigned(toFind, 16);
        char output[32];
        getFromRAM(num, 4, output);
        int outputNum = binaryToIntUnsigned(output, 32);
        writeToRAM(outputNum, 2, reg2);
    }

    else if(operand == 9){
        //loads the value at two lines above the current line into register 1
        int PCInt = binaryToIntUnsigned(PC, 16);
        PCInt -= 2;
        char output[16];
        getFromRAM(PCInt, 2, reg1);
    }
    else if(operand == 10){
        //loads the value at two lines above the current line into register 2
        int PCInt = binaryToIntUnsigned(PC, 16);
        PCInt -= 2;
        char output[16];
        getFromRAM(PCInt, 2, reg2);
    }


}

void die(){
    printf("shutting down");
    exit(0);
}

void decimalToBinary(int decimal, char *binary, int length){
    //converts a decimal int into a string of 1's and 0's of a specified length
    int index = 0;
    for(int i = length-1; i >= 0; i--){
        int possibleNum = twoToThePowerOf(i);
        if(possibleNum <= decimal){
            binary[index] = '1';
            decimal -= possibleNum;
        }
        else{
            binary[index] = '0';
        }
        index++;
    }
}

void setPC(char *setTo){
    for(int i = 0; i < 16; i++){
        PC[i] = setTo[i];
    }
}

void nop(){
    printf("nop\n");
    return;
}

void writeToMemory(int lineNum, char *toWrite, int length, int size){
    //this takes up like 0.4MB out of the 1MB avaliable on the stack so be careful
    //stores all the text in front of and behind a target byte array in Main Memory.txt changes the middle thing to whatever it's meant to be set to, and then writes the new array
    //starts at 0
    printf("lineNum: %d\n", lineNum);
    Sleep(2);
    if(size != ((sizeof(char) * length) * 8)){
        printf("error: in write to memory, the size of toWrite is different than length\n");
        printf("size of to write: %d\nsize of length: %d\n", size, (sizeof(char) * length * 8));
        exit(1);
    }
    int const FILE_LENGTH = FILE_SIZE; //file length in 8 byte (character, 0's and 1's) groups
    FILE *fh;
    fh = fopen("Main Memory.txt", "r");
    bool noSpaceBehind = false;
    bool noSpaceInFront = false;
    if(lineNum == 0){
        noSpaceBehind = true;
    }
    else if(lineNum + length == FILE_LENGTH){
        noSpaceInFront = true;
    }
    int counter = 0;
    if(noSpaceBehind){
        fseek(fh, 8, SEEK_SET);
        char spaceAhead[FILE_LENGTH * 8];
        char *pointsTospaceAhead = spaceAhead;
        fseek(fh, length*8, SEEK_SET);
        for(int i = 0; i < length * 8; i++){
            *pointsTospaceAhead = toWrite[i];
            pointsTospaceAhead++;
        }
        for(int i = 0; i < FILE_LENGTH - length; i++){
            fgets(pointsTospaceAhead, 9, fh);
            pointsTospaceAhead += 8;
        }
        fclose(fh);


        fh = fopen("Main Memory.txt", "w");
        fprintf(fh, spaceAhead);
        fclose(fh);

    }

    else if(noSpaceInFront){
        FILE *fp;
        fseek(fh, 8, SEEK_SET);
        char spaceBehind[FILE_LENGTH * 8];
        char *pointsToSpaceBehind = spaceBehind;
        fseek(fh, length*8, SEEK_SET);
        for(int i = 0; i < FILE_LENGTH - length; i++){
            fgets(pointsToSpaceBehind, 9, fh);
            pointsToSpaceBehind += 8;
        }

        for(int i = 0; i < length * 8; i++){
            *pointsToSpaceBehind = toWrite[i];
            pointsToSpaceBehind++;
        }
        fclose(fh);
        fh = fopen("Main Memory.txt", "w");
        fprintf(fh, spaceBehind);
        fclose(fh);

    }

    else{
        char spaceBehind[FILE_LENGTH * 8 + 1];
        char *pointsToSpaceBehind = spaceBehind;
        int counter;
        for(int i = 0; i < lineNum; i++){
            fgets(pointsToSpaceBehind, 9, fh);
            pointsToSpaceBehind += 8;
            counter += 8;
        }
        fseek(fh, counter, SEEK_SET);
        for(int i = 0; i < length * 8; i++){
            *pointsToSpaceBehind = toWrite[i];
            pointsToSpaceBehind++;
        }
        fseek(fh, lineNum * 8 + length * 8, SEEK_SET);
        for(int i = 0; i < FILE_LENGTH - (lineNum + length); i++){
            fgets(pointsToSpaceBehind, 9, fh);
            pointsToSpaceBehind += 8;
        }

        fclose(fh);
        *pointsToSpaceBehind = '\0';
        fh = fopen("Main Memory.txt", "w");
        fprintf(fh, spaceBehind);
        fclose(fh);

    }

    fclose(fh);

}


int twoToThePowerOf(int num){
    //returns two to the power of the parameter num

    if(num == 0){
        return 1;
    }
    int returnNum = 2;
    for(int i = 1; i < num; i++){
        returnNum *= 2;
    }

    return returnNum;
}

int countThingsInArray(char *toCheck){
    int index = 0;
    char character = toCheck[index];
    while(character != '\0'){
        index++;
        character = toCheck[index];
    }
    return index;
}

int binaryToIntUnsigned(char *string, int size){
    //takes an 8 bit binary number and returns its integer value (unisgned)
    int returnNum = 0;
    int index = size;
    for(int i = 0; i <= size-1; i++){
        if(string[i] == '1'){
            returnNum += twoToThePowerOf(index-1);
        }
        else{
        }
        index--;
    }
    return returnNum;
}

void fillWithZeros(){
    //fills the Main Memory.txt file with zeros
    int const FILE_LENGTH = FILE_SIZE; //file length in 8 byte groups
    FILE *fp;
    fp = fopen("Main Memory.txt", "w");
    for(int i = 0; i < FILE_LENGTH; i++){
        fprintf(fp, "00000000");
    }
    fclose(fp);

}

int countZeros(){
    //counts the number of characters in Main Memory.txt (all chars, not just zeros, I just used it to count zeros)
    int const FILE_LENGTH = (FILE_SIZE + 200) * 8; //File length in bytes, has to be 1 bigger than the actual length for some mystical reason??
    FILE *fp;
    fp = fopen("Main Memory.txt", "r");
    char toStore[FILE_LENGTH + 1];
    fgets(toStore, FILE_LENGTH, fp);
    int index = 0;
    int printNum = 0;
    char character = toStore[index];
    while(character != '\0'){
        printNum++;
        index++;
        character = toStore[index];
    }


    return printNum;
}

void incramentPC(int PCInt){
    //incarments tje program counter using the
    PCInt++;
    char setPCTo[16];
    decimalToBinary(PCInt, setPCTo, 16);
    setPC(setPCTo);

}

void getFromRAM(int lineStartAt, int linesToGet, char *output){
    //reads from RAM and copys data into the output string
    Sleep(1);
    int currentLine = lineStartAt;
    for(int i = 0; i < linesToGet; i++){
        for(int y = 0; y < 8; y++){
            output[i*8 + y] = RAM[currentLine][y];
        }
        currentLine++;
    }
}

void writeToRAM(int lineStartAt, int linesToWrite, char *toWrite){
    Sleep(1);
    //writes a spceified number of lines to a speciifed location in RAM
    int index = 0;
    for(int i = lineStartAt; i < linesToWrite + lineStartAt; i++){
        if(i == RAM_SIZE){
            flags[4] = '1'; //sets the memory error flag
            return;
        }
        for(int y = 0; y < 8; y++){
            RAM[i][y] = toWrite[index * 8 + y];
        }
        index++;
    }
}

void fillRAMWithZeros(){
    for(int i = 0; i < RAM_SIZE; i++){
        for(int y = 0; y < 8; y++){
            RAM[i][y] = '0';
        }
    }
}

int countRamZeros(){
    //counts the number of characters before null byte in RAM. dosen't just count zeros, I just used it for that
    char character = RAM[0][0];
    int i = 0;
    int y = 0;
    while(character != '\0'){
        for(i = 0; i < RAM_SIZE; i++){
            for(y = 0; y < 8; y++){
                character = RAM[i][y+1];
            }
            y = 0;
            character = RAM[i+1][0];
        }
        return i * 8 + y;
    }
}

void start(){
    fillRAMWithZeros();
}

bool cmpFunction(char *char1, char *char2, int length){
    //function to test if two strings are equivilent
    for(int i = 0; i < length; i++){
        if(char1[i] != char2[i]){
            return false;
        }
    }
    return true;
}


void cmp(char *opcode){
    //compares two 16 bit values and updates the equals flag based on there value
    char smallStr[5];
    substrAny(opcode, smallStr, 5, 11);
    int operand = binaryToIntUnsigned(smallStr, 5);
    if(operand == 0){
        //compares the contents of register 1 and register 2
        bool isEqual = cmpFunction(reg1, reg2, 16);
        if(isEqual){
            flags[1] = '1';
        }
        else{
            flags[1] = '0';
        }
    }
}

void not(char *opcode){
    //does the logical not operation to a specified register
    char smallStr[5];
    substrAny(opcode, smallStr, 5, 11);
    int operand = binaryToIntUnsigned(smallStr, 5);
    if(operand == 0){
        //does the logical not operation to register 1
        for(int i = 0; i < 16; i++){
            if(reg1[i] == '1'){
                reg1[i] = '0';
            }
            else if(reg1[i] == '0'){
                reg1[i] = '1';
            }
        }
    }
}

void and(char *opcode){
    char smallStr[5];
    substrAny(opcode, smallStr, 5, 11);
    int operand = binaryToIntUnsigned(smallStr, 5);
    if(operand == 0){
        for(int i = 0; i < 16; i++){
            if(reg1[i] == '1' && reg2[i] == '1'){
                reg1[i] = '1';
            }
            else{
                reg1[i] = '0';
            }
        }
    }
}

void or(char *opcode){
    char smallStr[5];
    substrAny(opcode, smallStr, 5, 11);
    int operand = binaryToIntUnsigned(smallStr, 5);
    if(operand == 0){
        for(int i = 0; i < 16; i++){
            if(reg1[i] == '1' || reg2[i] == '1'){
                reg1[i] = '1';
            }
            else{
                reg1[i] = '0';
            }
        }
    }

}

int opcodeToInt(char *opcode){
    //converts a character string of 1's and 0's called opcode into a integer number
    //retruns the integer number
    char smallStr[5];
    substrAny(opcode, smallStr, 5, 11);
    int operand = binaryToIntUnsigned(smallStr, 5);

    return operand;

}

void xor(char *opcode){
    char smallStr[5];
    substrAny(opcode, smallStr, 5, 11);
    int operand = binaryToIntUnsigned(smallStr, 5);
    if(operand == 0){
        for(int i = 0; i < 16; i++){
            if(reg1[i] != reg2[i]){
                reg1[i] = '1';
            }
            else{
                reg1[i] = '0';
            }
        }
    }

}

void addRecursive(char *add1, char *add2, int target, bool carry){
    //carry is the carry bit of binary addition
    //target is the target part of the two arrays. It starts at 0 and incarments by 1
    if(add1[target] == 1 && add2[target] == 1){
        if(carry = false){
            carry = true;
            add1[target] = '1';
        }
        else{
            add1[target] = '1';
        }

    if(add1[target] == 1 || add2[target] == 1){
        add1[target] = '1';
        }
    else if(carry == true){
        add1[target] = '1';
        }
    else{
        add1[target] = '0';
    }
    addRecursive(add1, add2, target++, carry);

    }

}

void jmp(char *opcode){
    //jumps two lines behind an address specified by a register, the idea is that the PC will be incramented by two lines
    int num = opcodeToInt(opcode);
    if(num == 0){
        int num2 = binaryToIntUnsigned(reg1, 16);
        num2 -= 2;
        decimalToBinary(num2, PC, 16);
        printf("PC (in jmp): ");
        printWithoutNullTerminator(PC, 16);
        printf("\n");
    }
    if(num == 1){
        int num2 = binaryToIntUnsigned(reg2, 16);
        num2 -= 2;
        decimalToBinary(num2, PC, 16);
    }

}

void add(char *opcode, char *add1, char *add2){
    int num = opcodeToInt(opcode);
    if(num == 0){
        addRecursive(add1, add2, 0, false);
    }
}

void setFlagsToZero(){
    for(int i = 0; i < 5; i++){
        flags[i] = '0';
    }
}

void printWithoutNullTerminator(char *output, int numOfChars){
    //does not print a new line
    for(int i = 0; i < numOfChars; i++){
        printf("%c", output[i]);
    }
}

void print(char *opcode){
    int intOpcode = opcodeToInt(opcode);
    if(intOpcode == 0){

    }
}

int main(){
    fillRAMWithZeros();
    int PCNum = 6;
    decimalToBinary(PCNum, PC, 16);
    char instruction[16] = "0100000000000001";
    char setReg2To[16] = "0000000000000100";
    setReg2(setReg2To, 16);
    char toRAM[16] = "0000100000000000";
    writeToRAM(4, 2, toRAM);
    doOp(instruction);
    PCNum = binaryToIntUnsigned(PC, 16);
    PCNum += 2;
    getFromRAM(PCNum, 2, instruction);
    doOp(instruction);
}
