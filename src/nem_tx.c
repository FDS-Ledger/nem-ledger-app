#include "nem_tx.h"
#include "nemHelpers.h"
#ifndef TESTING
#include "os.h"
#else
#include "lcx_ecfp.h"
#include <stdio.h>

#define SPRINTF(strbuf, ...) snprintf(strbuf, sizeof(strbuf), __VA_ARGS__)
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static const uint8_t AMOUNT_MAX_SIZE = 17;

//todo nonprintable ch + utf8
static void uint2Ascii(const uint8_t *inBytes, uint8_t len, char *out){
    const char *tmpCh = (const char *)inBytes;
    for (uint8_t j=0; j<len; j++){
        out[j] = tmpCh[j];
    }
    out[len] = '\0';
}

/** Convert 1 hex byte to 2 characters */
static char hex2Ascii(uint8_t input){
    return input > 9 ? (char)(input + 87) : (char)(input + 48);
}

static int compare_strings (const char str1[], const char str2[]) {
    int index = 0;
 
    while (str1[index] == str2[index]) {
        if (str1[index] == '\0' || str2[index] == '\0')
            break;
        index++;
    }
    
    if (str1[index] == '\0' && str2[index] == '\0')
        return 0;
    else
        return -1;
}

static int string_length(const char str[]) {
    int index = 0;
 
    while (str[index] != '\0') {
        str++;
    }
 
    return index;
}

static int print_amount(uint64_t amount, uint8_t divisibility, const char *asset, char *out) {
    char buffer[AMOUNT_MAX_SIZE];
    uint64_t dVal = amount;
    int i, j;

    // If the amount can't be represented safely in JavaScript, signal an error
    //if (MAX_SAFE_INTEGER < amount) THROW(0x6a80);

    memset(buffer, 0, AMOUNT_MAX_SIZE);
    for (i = 0; dVal > 0 || i < 7; i++) {
        if (dVal > 0) {
            buffer[i] = (dVal % 10) + '0';
            dVal /= 10;
        } else {
            buffer[i] = '0';
        }
        if (i == divisibility - 1) { // divisibility
            i += 1;
            buffer[i] = '.';
            if (dVal == 0) {
                i += 1;
                buffer[i] = '0'; 
            }           
        }
        if (i >= AMOUNT_MAX_SIZE) {
            return -1;
        }
    }
    // reverse order
    for (i -= 1, j = 0; i >= 0 && j < AMOUNT_MAX_SIZE-1; i--, j++) {
        out[j] = buffer[i];
    }
    // strip trailing 0s
    for (j -= 1; j > 0; j--) {
        if (out[j] != '0') break;
    }
    j += 1;

    // strip trailing .
    if (out[j-1] == '.') j -= 1;

    if (asset) {
        // qualify amount
        out[j++] = ' ';
        strcpy(out + j, asset);
        out[j+strlen(asset)] = '\0';
    } else {
        out[j] = '\0';
    }
    return 0;
}

int parse_transfer_tx (const uint8_t *raw_tx,
    size_t tx_len,
    unsigned int* ux_step_count, 
    char detailName[MAX_PRINT_DETAIL_NAME_SCREEN][MAX_PRINT_DETAIL_NAME_LENGTH],
    char extraInfo[MAX_PRINT_EXTRA_INFO_SCREEN][MAX_PRINT_EXTRA_INFOR_LENGTH],
    char fullAddress[NEM_ADDRESS_LENGTH],
    bool isMultisig) {
    
    //Fee
    uint64_t fee;

    //msg
    uint16_t lengthOfMessFeildIndex;
    uint32_t lengthOfMessFeild;
    uint16_t msgSizeIndex;
    uint32_t msgSize;
    uint16_t msgTypeIndex;
    uint16_t msgIndex;
    uint32_t msgType;
    char msg[MAX_PRINT_MESSAGE_LENGTH + 1];

    //mosaics
    uint16_t numberOfMosaicsIndex;
    uint8_t numberOfMosaics; 
    uint16_t mosaicIndex;

    //amount
    uint16_t amountIndex;
    uint32_t amount; 

    //Namespace ID
    uint16_t lengthOfIDIndex;
    uint32_t lengthOfID;
    uint16_t IDNameIndex;

    //Mosaic Name
    uint16_t lengthOfNameIndex;
    uint32_t lengthOfName;
    uint16_t nameIndex;
    char IDName[MAX_PRINT_EXTRA_INFOR_LENGTH];
    char name[MAX_PRINT_EXTRA_INFOR_LENGTH];

    //Quantity
    uint32_t quantity;

    //Array index
    uint8_t arrayIndex; 

    *ux_step_count = 5;

    //Address
    SPRINTF(detailName[0], "%s", "Recipient");
    uint2Ascii(&raw_tx[4+4+4+4+32+4+4+4+4], 40, fullAddress); 

    //Message
    SPRINTF(detailName[1], "%s", "Message");
    lengthOfMessFeildIndex = 4+4+4+4+32+4+4+4+4+40+4+4;
    lengthOfMessFeild = get_uint32_le(&raw_tx[lengthOfMessFeildIndex]);
    msgSizeIndex = lengthOfMessFeild == 0 ? 0 : lengthOfMessFeildIndex+4+4;
    msgSize = lengthOfMessFeild == 0 ? 0 : get_uint32_le(&raw_tx[msgSizeIndex]);
    msgTypeIndex = lengthOfMessFeildIndex+4;
    msgIndex = lengthOfMessFeildIndex+4+4+4;
    msgType = get_uint32_le(&raw_tx[msgTypeIndex]);
    if (lengthOfMessFeild == 0) {
        SPRINTF(extraInfo[0], "%s", "<empty msg>");
    }
    else if(msgType == 1) {
        if (raw_tx[msgIndex] == 0xFE) {
            SPRINTF(detailName[1], "%s", "Hex message");
            msgIndex++;
            for (arrayIndex = 0; (arrayIndex < msgSize - 1) && (arrayIndex*2 < 12); arrayIndex++) {
                msg[2*arrayIndex] = hex2Ascii((raw_tx[msgIndex + arrayIndex] & 0xf0) >> 4);
                msg[2*arrayIndex + 1] = hex2Ascii(raw_tx[msgIndex + arrayIndex] & 0x0f);
                msg[2*arrayIndex + 2] = '\0';
            }
            if ( arrayIndex*2 + 1> MAX_PRINT_MESSAGE_LENGTH) {
                SPRINTF(extraInfo[0], "%s ...", msg);
            } else {
                SPRINTF(extraInfo[0], "%s", msg);
            }
        } else if (msgSize > MAX_PRINT_MESSAGE_LENGTH) {
            uint2Ascii(&raw_tx[msgIndex], MAX_PRINT_MESSAGE_LENGTH, msg);
            SPRINTF(extraInfo[0], "%s ...", msg);
        } else {
            uint2Ascii(&raw_tx[msgIndex], msgSize, msg);
            SPRINTF(extraInfo[0], "%s", msg);
        }
    } else {
        SPRINTF(extraInfo[0], "%s", "<encrypted msg>");
    }

    //Fee
    SPRINTF(detailName[2], "%s", "Fee");
    fee = get_uint32_le(&raw_tx[4+4+4+4+32]);
    if (isMultisig) {
        fee += 150000;
    }
    if (print_amount(fee, 6, "xem", extraInfo[1])) {
        return -1;
    }

    //mosaics
    numberOfMosaicsIndex = lengthOfMessFeild == 0 ? lengthOfMessFeildIndex+4: lengthOfMessFeildIndex+4+4+4+msgSize;
    numberOfMosaics = get_uint32_le(&raw_tx[numberOfMosaicsIndex]);
    mosaicIndex = numberOfMosaicsIndex+4;
    
    //amount
    SPRINTF(detailName[3], "%s", "Amount");
    if (numberOfMosaics == 0) {
        amountIndex = 4+4+4+4+32+4+4+4+4+40;
        amount = get_uint32_le(&raw_tx[amountIndex]);
        if (print_amount(amount, 6, "xem", extraInfo[2])) {
            return -1;
        }
    } else {
        SPRINTF(extraInfo[2], "<find %d mosaics>", numberOfMosaics);
        
        //Show all mosaics on Ledger
        for (arrayIndex = 0; arrayIndex < numberOfMosaics; arrayIndex++) {
            //Namespace ID
            lengthOfIDIndex = mosaicIndex+4+4;
            lengthOfID = get_uint32_le(&raw_tx[lengthOfIDIndex]);
            IDNameIndex = mosaicIndex+4+4+4;
            mosaicIndex = IDNameIndex + lengthOfID;
            uint2Ascii(&raw_tx[IDNameIndex], lengthOfID, IDName);

            //Mosaic Name
            lengthOfNameIndex = mosaicIndex;
            lengthOfName = get_uint32_le(&raw_tx[lengthOfNameIndex]);
            nameIndex = lengthOfNameIndex+4;
            mosaicIndex = nameIndex + lengthOfName;
            uint2Ascii(&raw_tx[nameIndex], lengthOfName, name);

            //Quantity
            quantity = get_uint32_le(&raw_tx[mosaicIndex]);
            *ux_step_count = *ux_step_count + 1;
            if ((compare_strings(IDName,"nem") == 0) && (compare_strings(name,"xem") == 0)) {
                SPRINTF(detailName[4 + arrayIndex], "%s %d", "Amount", 1 + arrayIndex);
                if (print_amount(quantity, 6, "xem", extraInfo[3 + arrayIndex])) {
                    return -1;
                }
            } else {
                SPRINTF(detailName[4 + arrayIndex], "%s %d", "Raw units", 1 + arrayIndex);
                if (string_length(name) < 13) {
                    SPRINTF(extraInfo[3 + arrayIndex], "%d %s", quantity, name);
                } else {
                    SPRINTF(extraInfo[3 + arrayIndex], "%d %s...", quantity, name);
                }
            }
            mosaicIndex += 8;
        }
    }
    return 0;
}

int parse_mosaic_definition_tx (const uint8_t *raw_tx,
    size_t tx_len,
    unsigned int* ux_step_count, 
    char detailName[MAX_PRINT_DETAIL_NAME_SCREEN][MAX_PRINT_DETAIL_NAME_LENGTH],
    char extraInfo[MAX_PRINT_EXTRA_INFO_SCREEN][MAX_PRINT_EXTRA_INFOR_LENGTH],
    char fullAddress[NEM_ADDRESS_LENGTH],
    bool isMultisig) {

    //Fee
    uint64_t fee;

    //msg
    uint16_t msgSizeIndex;
    uint32_t msgSize;
    uint16_t msgIndex;
    char msg[MAX_PRINT_MESSAGE_LENGTH + 1];

    //amount
    uint16_t amountIndex;
    uint32_t amount; 

    //Namespace ID
    uint16_t lengthOfIDIndex;
    uint32_t lengthOfID;
    uint16_t IDNameIndex;

    //Mosaic Name
    uint16_t lengthOfNameIndex;
    uint32_t lengthOfName;
    uint16_t nameIndex;
    char name[MAX_PRINT_EXTRA_INFOR_LENGTH];

    //Requires Levy
    uint16_t levySizeIndex;
    uint32_t levySize;

    *ux_step_count = 11;

    //Namespace ID
    SPRINTF(detailName[0], "%s", "Namespace");
    lengthOfIDIndex = 16+32+16+32+4+4;
    lengthOfID = get_uint32_le(&raw_tx[lengthOfIDIndex]);
    IDNameIndex= lengthOfIDIndex+4;
    uint2Ascii(&raw_tx[IDNameIndex], lengthOfID, fullAddress);

    //Mosaic Name
    SPRINTF(detailName[1], "%s", "Mosaic Name");
    lengthOfNameIndex = IDNameIndex + lengthOfID;
    lengthOfName = get_uint32_le(&raw_tx[lengthOfNameIndex]);
    nameIndex = lengthOfNameIndex+4;
    uint2Ascii(&raw_tx[nameIndex], lengthOfName, name);
    SPRINTF(extraInfo[0], "%s", name);

    //Fee
    SPRINTF(detailName[2], "%s", "Fee");
    fee = get_uint32_le(&raw_tx[4+4+4+4+32]);
    if (isMultisig) {
        fee += 150000;
    }
    if (print_amount(fee, 6, "xem", extraInfo[1])) {
        return -1;
    }
    
    //Description
    SPRINTF(detailName[4], "%s", "Description");
    msgSizeIndex = nameIndex+lengthOfName;
    msgSize = get_uint32_le(&raw_tx[msgSizeIndex]);
    msgIndex = msgSizeIndex+4;
    if(msgSize > MAX_PRINT_MESSAGE_LENGTH){
        uint2Ascii(&raw_tx[msgIndex], MAX_PRINT_MESSAGE_LENGTH, msg);
        SPRINTF(extraInfo[3], "%s...", msg);
    } else {
        uint2Ascii(&raw_tx[msgIndex], msgSize, msg);
        SPRINTF(extraInfo[3], "%s", msg);
    }

    //Start Properties
    //divisibility
    SPRINTF(detailName[6], "%s", "Divisibility");
    msgIndex = msgIndex + msgSize + 4+4+4+12+4;
    uint2Ascii(&raw_tx[msgIndex], 1, msg);
    SPRINTF(extraInfo[5], "%s", msg);

    //initial Supply
    SPRINTF(detailName[5], "%s", "Initial Supply");
    msgSizeIndex = msgIndex+1 + 4+4+13;
    msgSize = get_uint32_le(&raw_tx[msgSizeIndex]);
    msgIndex = msgSizeIndex + 4;
    uint2Ascii(&raw_tx[msgIndex], msgSize, msg);
    SPRINTF(extraInfo[4], "%s", msg);

    //Transferable
    SPRINTF(detailName[7], "%s", "Mutable Supply");
    msgSizeIndex = msgIndex+msgSize + 4+4+13;
    msgSize = get_uint32_le(&raw_tx[msgSizeIndex]);
    msgIndex = msgSizeIndex + 4;
    uint2Ascii(&raw_tx[msgIndex], msgSize, msg);
    SPRINTF(extraInfo[6], "%s", compare_strings(msg, "true") == 0 ? "Yes" : "No");

    //Mutable Supply
    SPRINTF(detailName[8], "%s", "Transferable");
    msgSizeIndex = msgIndex+msgSize + 4+4+12;
    msgSize = get_uint32_le(&raw_tx[msgSizeIndex]);
    msgIndex = msgSizeIndex + 4;
    uint2Ascii(&raw_tx[msgIndex], msgSize, msg);
    SPRINTF(extraInfo[7], "%s", compare_strings(msg, "true") == 0 ? "Yes" : "No");

    //Requires Levy
    SPRINTF(detailName[9], "%s", "Requires Levy");
    levySizeIndex = msgIndex+msgSize;
    levySize = get_uint32_le(&raw_tx[levySizeIndex]);
    SPRINTF(extraInfo[8], "%s", levySize == 0 ? "No" : "Yes");

    //Rental Fee
    SPRINTF(detailName[3], "%s", "Rental Fee");
    amountIndex = levySizeIndex+levySize + 4+4+40;
    amount = get_uint32_le(&raw_tx[amountIndex]);
    if (print_amount(amount, 6, "xem", extraInfo[2])) {
        return -1;
    }
    //End Properties
    return 0;
}

int parse_mosaic_supply_change_tx (const uint8_t *raw_tx,
    size_t tx_len,
    unsigned int* ux_step_count, 
    char detailName[MAX_PRINT_DETAIL_NAME_SCREEN][MAX_PRINT_DETAIL_NAME_LENGTH],
    char extraInfo[MAX_PRINT_EXTRA_INFO_SCREEN][MAX_PRINT_EXTRA_INFOR_LENGTH],
    char fullAddress[NEM_ADDRESS_LENGTH],
    bool isMultisig) {

    //Fee
    uint64_t fee;

    //Namespace ID
    uint16_t lengthOfIDIndex;
    uint32_t lengthOfID;
    uint16_t IDNameIndex;

    //Mosaic Name
    uint16_t lengthOfNameIndex;
    uint32_t lengthOfName;
    uint16_t nameIndex;
    char name[MAX_PRINT_EXTRA_INFOR_LENGTH];

    //Supply type
    uint8_t supplyType; 

    //Quantity
    uint32_t quantity;
    
    *ux_step_count = 5;

    //Namespace ID
    SPRINTF(detailName[0], "%s", "Namespace");
    lengthOfIDIndex = 16+32+12+4;
    lengthOfID = get_uint32_le(&raw_tx[lengthOfIDIndex]);
    IDNameIndex= lengthOfIDIndex+4;
    uint2Ascii(&raw_tx[IDNameIndex], lengthOfID, fullAddress);

    //Mosaic Name
    SPRINTF(detailName[1], "%s", "Mosaic Name");
    lengthOfNameIndex = IDNameIndex + lengthOfID;
    lengthOfName = get_uint32_le(&raw_tx[lengthOfNameIndex]);
    nameIndex = lengthOfNameIndex+4;
    uint2Ascii(&raw_tx[nameIndex], lengthOfName, name);
    SPRINTF(extraInfo[0], "%s", name);

    //Fee
    SPRINTF(detailName[2], "%s", "Fee");
    fee = get_uint32_le(&raw_tx[4+4+4+4+32]);
    if (isMultisig) {
        fee += 150000;
    }
    if (print_amount(fee, 6, "xem", extraInfo[1])) {
        return -1;
    }

    //Supply type
    supplyType = get_uint32_le(&raw_tx[nameIndex+lengthOfName]);
    quantity = get_uint32_le(&raw_tx[nameIndex+lengthOfName+4]);
    if (supplyType == 0x01) {   //Increase supply
        SPRINTF(detailName[3], "%s", "Increase");
    } else { //Decrease supply 
        SPRINTF(detailName[3], "%s", "Decrease");
    }
    SPRINTF(extraInfo[2], "%d", quantity);
    return 0;
}

int parse_provision_namespace_tx (const uint8_t *raw_tx,
    size_t tx_len,
    unsigned int* ux_step_count, 
    char detailName[MAX_PRINT_DETAIL_NAME_SCREEN][MAX_PRINT_DETAIL_NAME_LENGTH],
    char extraInfo[MAX_PRINT_EXTRA_INFO_SCREEN][MAX_PRINT_EXTRA_INFOR_LENGTH],
    char fullAddress[NEM_ADDRESS_LENGTH],
    bool isMultisig) {

    //Fee
    uint64_t fee;

    //msg
    uint16_t msgSizeIndex;
    uint32_t msgSize;
    uint16_t msgIndex;
    char msg[MAX_PRINT_MESSAGE_LENGTH + 1];

    //Quantity
    uint16_t quantityIndex;
    uint32_t quantity;

    *ux_step_count = 6;

    //Sink Address
    SPRINTF(detailName[0], "%s", "Sink Address");
    uint2Ascii(&raw_tx[4+4+4+4+32+4+4+4+4], 40, fullAddress); 

    //Rental Fee
    SPRINTF(detailName[1], "%s", "Rental Fee");
    quantityIndex = 4+4+4+4+32+4+4+4+4+40;
    quantity = get_uint32_le(&raw_tx[quantityIndex]);
    if (print_amount(quantity, 6, "xem", extraInfo[0])) {
        return -1;
    }

    //Fee
    SPRINTF(detailName[2], "%s", "Fee");
    fee = get_uint32_le(&raw_tx[4+4+4+4+32]);
    if (isMultisig) {
        fee += 150000;
    }
    if (print_amount(fee, 6, "xem", extraInfo[1])) {
        return -1;
    }

    //Namespace
    SPRINTF(detailName[3], "%s", "Namespace");
    msgSizeIndex = quantityIndex + 8;
    msgSize = get_uint32_le(&raw_tx[msgSizeIndex]);
    msgIndex = msgSizeIndex + 4;
    uint2Ascii(&raw_tx[msgIndex], msgSize, msg);
    SPRINTF(extraInfo[2], "%s", msg);

    //Parent namespace
    SPRINTF(detailName[4], "%s", "Parent Name");
    msgSizeIndex = msgIndex + msgSize;
    msgSize = get_uint32_le(&raw_tx[msgSizeIndex]);
    if (msgSize == -1) {
        SPRINTF(extraInfo[3], "%s", "<New namespace>"); 
    } else {
        msgIndex = msgSizeIndex + 4;
        uint2Ascii(&raw_tx[msgIndex], msgSize, msg);
        SPRINTF(extraInfo[3], "%s", msg);
    }
    return 0;
}

int parse_aggregate_modification_tx (const uint8_t *raw_tx,
    size_t tx_len,
    unsigned int* ux_step_count,
    char detailName[MAX_PRINT_DETAIL_NAME_SCREEN][MAX_PRINT_DETAIL_NAME_LENGTH],
    char extraInfo[MAX_PRINT_EXTRA_INFO_SCREEN][MAX_PRINT_EXTRA_INFOR_LENGTH],
    char fullAddress[NEM_ADDRESS_LENGTH],
    bool isMultisig,
    uint8_t networkId) {

    //Fee
    uint64_t fee;

    //Cosign Address
    uint16_t    numOfCosigModificationIndex;
    uint8_t     numOfCosigModification;
    uint16_t    publicKeyIndex;
    char        address[40];
    uint16_t    typeOfModificationIndex;
    uint8_t     typeOfModification;
    uint8_t     index;

    //Min signatures
    uint16_t    minSigIndex;
    int8_t      minSig;

    *ux_step_count = 3;

    //Multisig Account
    publicKeyIndex = 4+4+4+4;
    public_key_to_address (networkId, &raw_tx[publicKeyIndex], address);
    if (isMultisig) { //Affected
        SPRINTF(detailName[0], "%s", "Edited account");
    } else { //Converted
        SPRINTF(detailName[0], "%s", "Converted Acc");
    }
    memset(fullAddress, 0, NEM_ADDRESS_LENGTH);
    memmove((void *)fullAddress, address, 40);

    //Cosignatures
    numOfCosigModificationIndex = 4+4+4+4+32+8+4;
    numOfCosigModification = get_uint32_le(&raw_tx[numOfCosigModificationIndex]);
    typeOfModificationIndex = numOfCosigModificationIndex + 4;

    for (index = 0; index < numOfCosigModification; index++) {
        *ux_step_count = *ux_step_count + 1;
        typeOfModificationIndex += 4;
        typeOfModification = get_uint32_le(&raw_tx[typeOfModificationIndex]);

        publicKeyIndex = typeOfModificationIndex +4+4;
        public_key_to_address (networkId, &raw_tx[publicKeyIndex], address);

        //Top line
        if (typeOfModification == 0x01) {
            SPRINTF(detailName[index+1], "%s", "Add cosign");
        } else {
            SPRINTF(detailName[index+1], "%s", "Remove cosign");
        }
        //Bottom line
        memset(extraInfo[index], 0, MAX_PRINT_EXTRA_INFOR_LENGTH);
        memmove((void *)extraInfo[index], address, 6);
        memmove((void *)(extraInfo[index] + 6), "~", 1);
        memmove((void *)(extraInfo[index] + 6 + 1), address + 40 - 4, 4);

        typeOfModificationIndex = typeOfModificationIndex + 4 + 4 + 32;
        numOfCosigModificationIndex = typeOfModificationIndex;
    }

    //Min signatures
    minSigIndex = numOfCosigModification == 0 ? numOfCosigModificationIndex + 4+4 : numOfCosigModificationIndex +4;
    minSig = get_uint32_le(&raw_tx[minSigIndex]);
    if (minSig > 0) {
        SPRINTF(detailName[numOfCosigModification+1], "%s", "Num of minsig");
        SPRINTF(extraInfo[numOfCosigModification], "Increase %d", minSig);
    } else if (minSig < 0) {
        SPRINTF(detailName[numOfCosigModification+1], "%s", "Num of minsig");
        SPRINTF(extraInfo[numOfCosigModification], "Decrease %d", ~minSig + 1);
    }
    if (minSig != 0) {
        numOfCosigModification += 1;
        *ux_step_count = *ux_step_count + 1;
    }

    //Fee
    SPRINTF(detailName[numOfCosigModification+1], "%s", "Fee");
    fee = get_uint32_le(&raw_tx[4+4+4+4+32]);
    if (isMultisig) {
        fee += 150000;
    }
    if (print_amount(fee, 6, "xem", extraInfo[numOfCosigModification])) {
        return -1;
    }
    return 0;
}

int parse_multisig_tx (const uint8_t *raw_tx,
    size_t tx_len,
    unsigned int* ux_step_count, 
    char detailName[MAX_PRINT_DETAIL_NAME_SCREEN][MAX_PRINT_DETAIL_NAME_LENGTH],
    char extraInfo[MAX_PRINT_EXTRA_INFO_SCREEN][MAX_PRINT_EXTRA_INFOR_LENGTH],
    char fullAddress[NEM_ADDRESS_LENGTH],
    uint8_t networkId) {
    int ret = -1;
    uint32_t otherTxType = get_uint32_le(&raw_tx[0]);

    switch (otherTxType) {
        case NEMV1_TRANSFER:
            ret = parse_transfer_tx (raw_tx,
                tx_len,
                ux_step_count, 
                detailName,
                extraInfo,
                fullAddress,
                true
            );
            break;
        case NEMV1_PROVISION_NAMESPACE:
            ret = parse_provision_namespace_tx (raw_tx,
                tx_len,
                ux_step_count, 
                detailName,
                extraInfo,
                fullAddress,
                true
            );
            break;
        case NEMV1_MOSAIC_DEFINITION:
            ret = parse_mosaic_definition_tx (raw_tx,
                tx_len,
                ux_step_count, 
                detailName,
                extraInfo,
                fullAddress,
                true
            );
            break;
        case NEMV1_MOSAIC_SUPPLY_CHANGE:
            ret = parse_mosaic_supply_change_tx (raw_tx,
                tx_len,
                ux_step_count, 
                detailName,
                extraInfo,
                fullAddress,
                true
            );
            break;
        case NEMV1_MULTISIG_MODIFICATION:
            ret = parse_aggregate_modification_tx (raw_tx,
                tx_len,
                ux_step_count, 
                detailName,
                extraInfo,
                fullAddress,
                true,
                networkId
            );
            break;
        default:
            break;
    }
    return ret;
}

int parse_multisig_signature_tx (const uint8_t *raw_tx,
    size_t tx_len,
    unsigned int* ux_step_count,
    char detailName[MAX_PRINT_DETAIL_NAME_SCREEN][MAX_PRINT_DETAIL_NAME_LENGTH],
    char extraInfo[MAX_PRINT_EXTRA_INFO_SCREEN][MAX_PRINT_EXTRA_INFOR_LENGTH],
    char fullAddress[NEM_ADDRESS_LENGTH]) {

    //Fee
    uint8_t multisigFeeIndex;
    uint64_t fee;

    //multisig Address
    uint8_t multisigAddressIndex;

    //Hash bytes
    uint8_t hashBytesIndex;
    char hashBytes[65];

    uint8_t index;

    *ux_step_count = 4;
    
    //Cosign transaction for
    SPRINTF(detailName[0], "%s", "Cosign tx for");
    multisigAddressIndex = 4+4+4+4+32+8+4+4+4+32+4;
    uint2Ascii(&raw_tx[multisigAddressIndex], 40, fullAddress);

    //Hash
    SPRINTF(detailName[1], "%s", "SHA hash");
    hashBytesIndex = 4+4+4+4+32+8+4+ 4+4;
    for (index = 0; index < 32; index++) {
        hashBytes[2*index] = hex2Ascii((raw_tx[index + hashBytesIndex] & 0xf0) >> 4);
        hashBytes[2*index + 1] = hex2Ascii(raw_tx[index + hashBytesIndex] & 0x0f);
    }
    memset(extraInfo[0], 0, MAX_PRINT_EXTRA_INFOR_LENGTH);
    memmove((void *)extraInfo[0], hashBytes, 6);
    memmove((void *)(extraInfo[0] + 6), "~", 1);
    memmove((void *)(extraInfo[0] + 6 + 1), hashBytes + 64 - 4, 4);

    //Multisig fee
    SPRINTF(detailName[2], "%s", "Multisig fee");
    multisigFeeIndex = 4+4+4+4+32;
    fee = get_uint32_le(&raw_tx[multisigFeeIndex]);
    if (print_amount(fee, 6, "xem", extraInfo[1])) {
        return -1;
    }
    return 0;
}
