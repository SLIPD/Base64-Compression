#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define CHARACTERS_LENGTH 62

char initialised = 0;
char *characters;




void initialise(){
	uint8_t tempChars[] = {'\0',	'A','B','C','D','E','F','G','H',
									'I','J','K','L','M','N','O','P',
									'Q','R','S','T','U','V','W','X',
									'Y','Z','0','1','2','3','4','5',
									'6','7','8','9',' ','.',',','<',
									'>','/','?',';',':','|','"','!',
									'@','#','%','&','*','(',')','-',
									'_','=','+','\'','$'};
	characters = malloc(sizeof(uint8_t) * CHARACTERS_LENGTH);
	memcpy(characters,tempChars,CHARACTERS_LENGTH);
	initialised = 1;
}


int stringLength(uint8_t* inputData){
	int byteNum = 0;
	int bitNum = 8;
	uint8_t current = 0;
	while(1){
    	if(bitNum == 8){
    		current = (inputData[byteNum] >> 2) & 0x3F;
    		bitNum = 2;
    	} else if(bitNum == 6) {
    		current = inputData[byteNum] & 0x3F;
    		byteNum++;
    		bitNum = 8;
    	} else if(bitNum == 4) {
    		current = (inputData[byteNum] << 2) & 0x3C;
    		byteNum++;
    		current = current | ((inputData[byteNum] >> 6) & 0x03);
    		bitNum = 6;
    	} else if(bitNum == 2) {
    		current = (inputData[byteNum] << 4) & 0x30;
    		byteNum++;
    		current = current | ((inputData[byteNum] >> 4) & 0x0F);
    		bitNum = 4;
    	}
    	if(!current) return byteNum;
    }
}

void printHexString(uint8_t* string){
	int stringLen = stringLength(string);
	int i = 0;
	for(i = 0; i < stringLen; i++){
		printf("%.2X ",(uint8_t)(*(string + i)));
	}
	printf("\n");
}

void printIntString(uint8_t* string){
	int stringLen = stringLength(string);
	int i = 0;
	for(i = 0; i < stringLen; i++){
		printf("%d ",(uint8_t)(*(string + i)));
	}
	printf("\n");
}


uint8_t findPosition(uint8_t* character, uint8_t* array, uint8_t arrayLength){
	int i = 0;
	for(i = 0; i < arrayLength; i++){
		uint8_t current = *character;
		if(current >=97 && current <= 123){
			current = current - 32;
		}
		if(current == array[i]){
			return i;
		}
	}
	printf("Character not found: %c\n",(char)(*character));
	return 0xFF;
}

uint8_t getCharacter(uint8_t index, uint8_t* array, uint8_t arrayLength){
	if(index > arrayLength) return 0xFF;
	return (uint8_t)(*(array + index));
}

uint8_t* encodeDataString(uint8_t *string){
    if(!initialised) initialise();
    int stringBitLength = strlen((char*)string) * 6;
    int outputLength = (int)ceil((double)stringBitLength / 8.0);
    if(stringBitLength % 8 == 0) outputLength++;
    outputLength++;
    uint8_t *returnData = malloc(sizeof(uint8_t) * outputLength);
    
    int i = 0;
    int byteNum = 0;
    int bitNum = 8;
    
    for(i = 0; i < outputLength; i++){
    	returnData[i] = '\0';
    }

    for(i = 0; i < strlen((char*)string); i++){
    	int value = findPosition((string + i),characters,CHARACTERS_LENGTH);
    	if(value == 0xFF) continue;
    	if(bitNum == 8){
    		returnData[byteNum] = (value << 2) & 0xFC;
    		bitNum = 2;
    	} else if(bitNum == 6) {
    		returnData[byteNum] = (returnData[byteNum] & 0xC0) | (value & 0x3F);
    		byteNum++;
    		bitNum = 8;
    	} else if(bitNum == 4) {
    		returnData[byteNum] = (returnData[byteNum] & 0xF0) | ((value >> 2) & 0x0F);
    		byteNum++;
    		returnData[byteNum] = (returnData[byteNum] & 0x3F) | ((value << 6) & 0xC0);
    		bitNum = 6;
    	} else if(bitNum == 2) {
    		returnData[byteNum] = (returnData[byteNum] & 0xFC) | ((value >> 4) & 0x03);
    		byteNum++;
    		returnData[byteNum] = (returnData[byteNum] & 0x0F) | ((value << 4) & 0xF0);
    		bitNum = 4;
    	}
    }
    
    return returnData;
    
}


uint8_t* decodeDataString(uint8_t *string){

    if(!initialised) initialise();
    int stringBitLength = stringLength(string) * 8;
    int outputLength = (int)ceil((double)stringBitLength / 6.0);
    if(stringBitLength % 6 == 0) outputLength++;
    outputLength++;
    
    uint8_t *inputData = malloc(stringLength(string) + 1);
    memcpy(inputData,string,stringLength(string));
    inputData[stringLength(string)] = '\0';
    
    uint8_t *returnData = malloc(sizeof(uint8_t) * outputLength);
    
    int i = 0;
    int byteNum = 0;
    int bitNum = 8;
    uint8_t current = 0;
    for(i = 0; i < outputLength; i++){
    	returnData[i] = '\0';
    }
    
    for(i = 0; i < outputLength; i++){
    	if(bitNum == 8){
    		current = (inputData[byteNum] >> 2) & 0x3F;
    		bitNum = 2;
    	} else if(bitNum == 6) {
    		current = inputData[byteNum] & 0x3F;
    		byteNum++;
    		bitNum = 8;
    	} else if(bitNum == 4) {
    		current = (inputData[byteNum] << 2) & 0x3C;
    		byteNum++;
    		current = current | ((inputData[byteNum] >> 6) & 0x03);
    		bitNum = 6;
    	} else if(bitNum == 2) {
    		current = (inputData[byteNum] << 4) & 0x30;
    		byteNum++;
    		current = current | ((inputData[byteNum] >> 4) & 0x0F);
    		bitNum = 4;
    	}
		uint8_t character = getCharacter(current,characters,CHARACTERS_LENGTH);
    	if(character == 0xFF) continue;
    	*(returnData + i) = (uint8_t)character;
    }
    
    return returnData;
    
}

void decodeData(uint8_t* data, char* message, char* debug){
	uint8_t* decodedData = decodeDataString(data);
	printf("Decoded data: %s\n",(char*)decodedData);
	char dataPos = 0;
	char messagePos = 0;
	char debugPos = 0;
	char debugMode = 0;
	while(dataPos < strlen((char*)decodeData)){
		if(debugMode){
			debug[debugPos] = decodedData[dataPos];
			debugPos++;
		} else {
			if(decodedData[dataPos] == '$'){
				debugMode = 1;
			} else {
				message[messagePos] = decodedData[dataPos];
				messagePos++;
			}
		}
		dataPos++;
	}
}

int main(int argc, char* argv[]){
	uint8_t *testString;
	if(argc > 1){
		testString = (uint8_t*)argv[1];
		printf("Test string: %s, Teststring length: %d\n",(char*)testString,(int)strlen((char*)testString));
		uint8_t* encodedString = encodeDataString(testString);
		printHexString(encodedString);
		char *decodedMessage = calloc(32,1);
		char *decodedDebug = calloc(31,1);
		decodeData(encodedString,decodedMessage,decodedDebug);
		printf("DECODED STRING: %s\n",decodedMessage);
		printf("DECODED DEBUG: %s\n",decodedDebug);
		
	}
}
	
	

