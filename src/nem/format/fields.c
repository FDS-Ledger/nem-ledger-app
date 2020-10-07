/*******************************************************************************
*   NEM Wallet
*    (c) 2020 FDS
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
#include "fields.h"
#include "common.h"
#include "limitations.h"

void resolve_fieldname(field_t *field, char* dst) {
    if (field->dataType == STI_INT8) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_INT8_MAM_REMOVAL_DELTA, "Min Removal")
            CASE_FIELDNAME(NEM_INT8_MAM_APPROVAL_DELTA, "Min Approval")
        }
    }

    if (field->dataType == STI_UINT32) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_UINT32_TRANSACTION_TYPE, "Transaction Type")
            CASE_FIELDNAME(NEM_UINT32_INNER_TRANSACTION_TYPE, "Inner TX Type")
            CASE_FIELDNAME(NEM_UINT32_MOSAIC_COUNT, "Mosaics")
        }
    }

    if (field->dataType == STI_UINT8) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_UINT8_TXN_MESSAGE_TYPE, "Message Type")
            CASE_FIELDNAME(NEM_UINT8_MOSAIC_COUNT, "Mosaics")
            CASE_FIELDNAME(NEM_UINT8_MSC_ACTION, "Change Direction")
            CASE_FIELDNAME(NEM_UINT8_NS_REG_TYPE, "Namespace Type")
            CASE_FIELDNAME(NEM_UINT8_AA_TYPE, "Alias Type")
            CASE_FIELDNAME(NEM_UINT8_MD_SUPPLY_FLAG, "Supply Change")
            CASE_FIELDNAME(NEM_UINT8_MD_TRANS_FLAG, "Transferable")
            CASE_FIELDNAME(NEM_UINT8_MD_RESTRICT_FLAG, "Restriction")
            CASE_FIELDNAME(NEM_UINT8_MAM_ADD_COUNT, "Address Add Num")
            CASE_FIELDNAME(NEM_UINT8_MAM_DEL_COUNT, "Address Del Num")
        }
    }

    if (field->dataType == STI_UINT64) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_UINT64_DURATION, "Duration")
            CASE_FIELDNAME(NEM_UINT64_PARENTID, "Parent ID")
            CASE_FIELDNAME(NEM_UINT64_MSC_AMOUNT, "Change Amount")
            CASE_FIELDNAME(NEM_UINT64_NS_ID, "Namespace ID")
            CASE_FIELDNAME(NEM_UINT64_MOSAIC_ID, "Mosaic ID")
        }
    }

    if (field->dataType == STI_HASH256) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_HASH256_AGG_HASH, "Agg. Tx Hash")
            CASE_FIELDNAME(NEM_HASH256_HL_HASH, "Tx Hash")
        }
    }

    if (field->dataType == STI_ADDRESS) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_STR_RECIPIENT_ADDRESS, "Recipient")
            CASE_FIELDNAME(NEM_STR_ADDRESS, "Address")
        }
    }

    if (field->dataType == STI_MOSAIC_CURRENCY) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_MOSAIC_AMOUNT, "Amount")
            CASE_FIELDNAME(NEM_MOSAIC_HL_QUANTITY, "Lock Quantity")
            CASE_FIELDNAME(NEM_MOSAIC_UNITS, "Micro Units")
        }
    }

    if (field->dataType == STI_NEM) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_UINT64_TXN_FEE, "Fee")
            CASE_FIELDNAME(NEM_MOSAIC_AMOUNT, "Amount")
        }
    }

    if (field->dataType == STI_MESSAGE) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_STR_TXN_MESSAGE, "Message")
        }
    }

    if (field->dataType == STI_STR) {
        switch (field->id) {
            CASE_FIELDNAME(NEM_STR_NAMESPACE, "Namespace ID")
            CASE_FIELDNAME(NEM_STR_MOSAIC, "Mosaic name")
        }
    }

    // Default case
    snprintf(dst, MAX_FIELDNAME_LEN, "Unknown Field");
}