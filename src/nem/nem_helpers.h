/*******************************************************************************
*   NEM Wallet
*   (c) 2020 FDS
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
#ifndef LEDGER_APP_NEM_NEMHELPERS_H
#define LEDGER_APP_NEM_NEMHELPERS_H

#include <os.h>
#include <cx.h>
#include <os_io_seproxyhal.h>
#include <stdbool.h>

#define NEM_TXN_TRANSFER 0x0101
#define NEM_TXN_IMPORTANCE_TRANSFER 0x0801
#define NEM_TXN_MULTISIG_AGGREGATE_MODIFICATION 0x1001
#define NEM_TXN_MULTISIG_SIGNATURE 0x1002
#define NEM_TXN_MULTISIG 0x1004
#define NEM_TXN_PROVISION_NAMESPACE 0x2001
#define NEM_TXN_MOSAIC_DEFINITION 0x4001
#define NEM_TXN_MOSAIC_SUPPLY_CHANGE 0x4002

#define NEM_TXN_REGISTER_NAMESPACE 0x414E
#define NEM_TXN_ADDRESS_ALIAS 0x424E
#define NEM_TXN_MOSAIC_ALIAS 0x434E
#define NEM_TXN_MODIFY_MULTISIG_ACCOUNT 0x4155
#define NEM_TXN_AGGREGATE_COMPLETE 0x4141
#define NEM_TXN_AGGREGATE_BONDED 0x4241
#define NEM_TXN_HASH_LOCK 0x4148
#define NEM_TXN_SECRET_LOCK 0x4152
#define NEM_TXN_SECRET_PROOF 0x4252
#define NEM_TXN_MODIFY_ACCOUNT_PROPERTY_ADDRESS 0x4150
#define NEM_TXN_MODIFY_ACCOUNT_PROPERTY_MOSAIC 0x4250
#define NEM_TXN_MODIFY_ACCOUNT_PROPERTY_ENTITY_TYPE 0x4350

#define AMOUNT_MAX_SIZE 17
#define NEM_ADDRESS_LENGTH 40
#define NEM_PRETTY_ADDRESS_LENGTH 40
#define NEM_PUBLIC_KEY_LENGTH 32
#define NEM_PRIVATE_KEY_LENGTH 32
#define NEM_TRANSACTION_HASH_LENGTH 32

#define TESTNET 152 //0x98
#define MAINNET 104 //0x68
#define MIJIN_MAINNET 96 //0x60
#define MIJIN_TESTNET 144 //0x90

uint8_t get_network_type(const uint32_t bip32Path[]);
uint8_t get_algo(uint8_t network_type);
void nem_print_amount(uint64_t amount, uint8_t divisibility, char *asset, char *out);
void nem_public_key_and_address(cx_ecfp_public_key_t *inPublicKey, uint8_t inNetworkId, unsigned int inAlgo, uint8_t *outPublicKey, char *outAddress, uint8_t outLen);

#endif //LEDGER_APP_NEM_NEMHELPERS_H