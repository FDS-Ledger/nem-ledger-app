# NEM's APDU package fields

### I. Common transaction part
```
01. CLA   (1 byte)
02. INS   (1 byte)
03. P1    (1 byte)
04. P2    (1 byte)
05. LC    (1 byte)
06. CDATA (1 byte)

(Reference: https://nemproject.github.io/#gathering-data-for-the-signature)

07. Devariant path (44/43/0/0/0)   (20 bytes) (8000002C8000002B800000988000000080000000) (fixed)
08. Transaction Type               (4 bytes)  (depends on tx) (01010000)
09. Version                        (4 bytes)  (depends on tx) (01000098) (depends on network and transaction type)
10. Timestamp                      (4 bytes)  (depends on tx) (9B5CD007)
11. Public key length              (4 bytes)  (fixed) (20000000)
12. Signer Public Key              (32 bytes)  (depends on tx) (3E6E6CBAC488B8A44BDF5ABF27B9E1CC2A6F20D09D550A66B9B36F525CA222EE)
13. Fee (micro xem)                (8 bytes)  (depends on tx) (A086010000000000)
14. Deadline                       (4 bytes)  (depends on tx) (AB6AD007)
```
### II. Properties parts (Phan rieng cua cac goi tin)

# Transfer transaction schema
1. Transfer transaction part (Reference: https://nemproject.github.io/#gathering-data-for-the-signature)

```
Property                                             Types                                           Description
----------------------------------------------------------------------------------------------------------------------
recipientAddressLength                               uint32           	                             Length of recipient address (always 40)
recipientAddress 	                                 40 Bytes                                        Transaction recipient (UTF8 encoding)
amount                                               uint64                       	                 Amount of sending xem  (micro xem)
messageFieldLength                                   uint32
MessageType  (Not exist if messageFieldLength = 0)   uint32 	                                     MessageType
LengthOfPayload(Not exist if messageFieldLength = 0) uint32
Payload (Not exist if messageFieldLength = 0)        array(messageSize)                              Hexadecimal payload.

(Optional: available for version 2 transfer transactions that have an attachment.)
mosaicsNum 	                                        uint32                                          Attached mosaics to send.
mosaicArray                                         Array(mosaicNum, mosaicStructure)               Mosaic structure array
```

1.1 Mosaic structure part
```
Property                                             Types                                           Description
----------------------------------------------------------------------------------------------------------------------
MosaicStructureLength                                uint32
MosaicIdStructureLength                              unit32
NamespaceIdLength                                    uint32
MosaicNameStringLength                               uint32
MosaicName                                           Array(MosaicNameStringLength)
Quantity                                             uint64
```



Example:

Full raw transaction (ledger receive):
```
E0049080A9058000002C8000002B80000098800000008000000001010000010000989B5CD007200000003E6E6CBAC488B8A44BDF5ABF27B9E1CC2A6F20D09D550A66B9B36F525CA222EEA086010000000000AB6AD007280000005441353435494341564E45554446554249484F3343454A425356495A37595948464658354C51505440420F00000000002000000001000000180000005369676E20746573746E6574207472616E73616374696F6E

```
#### Parsed above tx
```
##### Common parts

01 -> 06                    E0049080A905
07                          8000002C8000002B800000988000000080000000
08                          01010000
09                          01000098
10                          9B5CD007
11                          20000000
12                          3E6E6CBAC488B8A44BDF5ABF27B9E1CC2A6F20D09D550A66B9B36F525CA222EE
13                          A086010000000000
14                          AB6AD007


##### Properties parts

recipientAddressLength      28000000
recipientAddress            5441353435494341564E45554446554249484F3343454A425356495A37595948464658354C515054
amount                      40420F0000000000
messageFieldLength          20000000
MessageType                 01000000
LengthOfPayload             18000000
Payload                     5369676E20746573746E6574207472616E73616374696F6E
```
E0040080A9058000002C8000002B80000098800000008000000001010000010000989B5CD007200000003E6E6CBAC488B8A44BDF5ABF27B9E1CC2A6F20D09D550A66B9B36F525CA222EEA086010000000000AB6AD007280000005441353435494341564E45554446554249484F3343454A425356495A37595948464658354C515054
40420F0000000000
20000000
01000000
18000000
5369676E20746573746E6574207472616E73616374696F6E


### Example: multi mosaic transaction
```
01010000
02000098
88AF640A
20000000
9F96DF7E7A639B4034B8BEE5B88AB1D640DB66EB5A47AFE018E320CB130C183D
F049020000000000
98BD640A

28000000
54423749423644534A4B57425651454B3750443754574F3636454357354C59365349534D32434A4A
40420F0000000000
14000000
01000000
0C000000
54657374206D657373616765

02000000

1A000000
0E000000
03000000
6E656D
03000000
78656D
40420F0000000000

20000000
14000000
07000000
746573746E6574
05000000
746F6B656E
0100000000000000
```
# Importance Transfer transaction schema
2. Importance Transfer transaction part (Reference: https://nemproject.github.io/#gathering-data-for-the-signature)

```
Property                                             Types                                         Description
----------------------------------------------------------------------------------------------------------------------
Importance transfer mode                            uint32                                         0x01 (activate) or 0x02  (Deactivate mode)
RemoteAccountPublicKeyArrayLength                   uint32                                         Length of remote account public key byte array
RemoteAccountPublicKeyArray                         Array(32 bytes, RemoteAccountPublicKeyArrayLength)
```

#  Provision namespace Transaction schema
3. Provision namespace Transaction (Reference: https://nemproject.github.io/#gathering-data-for-the-signature)

```
Property                                Types                                       Description
----------------------------------------------------------------------------------------------------------------------
feeSinkEncodedAddressLength             uint32                                     	(fixed) (0x28)
feeSinkEncodedAddress                   40 bytes	                                Address bytes of rental fee sink
Rental fee  	                        8 bytes 	                                (fixed) (Root always: 100000000, Sub always: 10000000)
newPartStringLength                     uint32
newPartString                           array(newPartStringLength)
parentStringLength                      uint32
parentString    (optional)              array(parentStringLength)                   (if parentStringLength = FFFFFFFF) this field is omitted
```
Example:

Raw transaction:
```
0120000001000098C7AF640A200000009F96DF7E7A639B4034B8BEE5B88AB1D640DB66EB5A47AFE018E320CB130C183DF049020000000000D7BD640A2800000054414D4553504143455748344D4B464D42435646455244504F4F5034464B374D54444A4559503335809698000000000012000000746573745F6E616D6573706163655F6E616D08000000746573745F6E656D
```

### Parse above transaction
```
08                                  01200000
09                                  01000098
10                                  C7AF640A
11                                  20000000
12                                  9F96DF7E7A639B4034B8BEE5B88AB1D640DB66EB5A47AFE018E320CB130C183D
13                                  F049020000000000
14                                  D7BD640A

feeSinkEncodedAddressLength         28000000
feeSinkEncodedAddress               54414D4553504143455748344D4B464D42435646455244504F4F5034464B374D54444A4559503335
Rental Fee                          8096980000000000
newPartStringLength                 12000000
newPartString                       746573745F6E616D6573706163655F6E616D
parentStringLength                  08000000
parentString    (optional)          746573745F6E656D
```
#  Mosaic Supply Change Transaction schema
4. Mosaic Supply Change Transaction (Reference: https://nemproject.github.io/#gathering-data-for-the-signature)

```
Property                                Types                                       Description
----------------------------------------------------------------------------------------------------------------------
mosaicIdLength                         	uint32
namespaceIdStringLength                 uint32
namespaceIdString                       array(namespaceIdStringLength)
mosaicNameStringLength                  uint32
mosaicNameString                        array(mosaicNameStringLength)
supplyType                              uint32                                      0x01 (increase), 0x02 (decrease)
delta                                   uint64
```



##  Mosaic definition creation transaction part schema
5. Mosaic definition creation transaction (Reference: https://nemproject.github.io/#gathering-data-for-the-signature)

Raw data:
```
0140000001000098b8b0640a200000009f96df7e7a639b4034b8bee5b88ab1d640db66eb5a47afe018e320cb130c183df049020000000000c8be640a22010000200000009f96df7e7a639b4034b8bee5b88ab1d640db66eb5a47afe018e320cb130c183d2000000008000000746573745f6e656d100000006d6f736169635f746573745f6e616d65170000007468697320697320746865206465736372697074696f6e04000000150000000c00000064697669736962696c6974790100000034170000000d000000696e697469616c537570706c79020000003235190000000d000000737570706c794d757461626c650400000074727565180000000c0000007472616e7366657261626c6504000000747275654a000000010000002800000054423749423644534a4b57425651454b3750443754574f3636454357354c59365349534d32434a4a0e000000030000006e656d0300000078656d09000000000000002800000054424d4f534149434f443446353445453543444d523233434342474f414d3258534a4252354f4c438096980000000000
```
```
01400000
01000098
b8b0640a
20000000
9f96df7e7a639b4034b8bee5b88ab1d640db66eb5a47afe018e320cb130c183d
f049020000000000
c8be640a


22010000
20000000
9f96df7e7a639b4034b8bee5b88ab1d640db66eb5a47afe018e320cb130c183d
20000000
08000000
746573745f6e656d
10000000
6d6f736169635f746573745f6e616d65
17000000
7468697320697320746865206465736372697074696f6e

04000000

15000000
0c000000
64697669736962696c697479
01000000
34

17000000
0d000000
696e697469616c537570706c79
02000000
3235

19000000
74727565


4a000000
01000000
28000000
54423749423644534a4b57425651454b3750443754574f3636454357354c59365349534d32434a4a
0e000000
03000000
6e656d
03000000
78656d
0900000000000000

28000000
54424d4f534149434f443446353445453543444d523233434342474f414d3258534a4252354f4c43
8096980000000000
```
## Multisig transaction part schema
6.  Multisig transaction part


```
Property                                Types                                       Description
----------------------------------------------------------------------------------------------------------------------
Inner transaction object Length         uint32_t                                     This can be a transfer, an importance transfer or an aggregate modification transaction.
```

Example:

Raw transaction:
```
04100000010000985560d007200000003e6e6cbac488b8a44bdf5abf27b9e1cc2a6f20d09d550a66b9b36f525ca222eef049020000000000656ed0077400000001010000010000985560d0072000000093ce7f61acd7250f98d9ceeab18281b26fcabbc8845a6749814851626bacbf5150c3000000000000656ed007280000005441353435494341564e45554446554249484f3343454a425356495a37595948464658354c51505440420f000000000000000000
```

### Parse above transaction
```
08                      04100000
09                      01000098
10                      5560d007
11                      20000000
12                      3e6e6cbac488b8a44bdf5abf27b9e1cc2a6f20d09d550a66b9b36f525ca222ee
13                      f049020000000000
14                      656ed007

InnerTransactionLength  74000000
08                      01010000
09                      01000098
10                      5560d007
11                      20000000
12                      93ce7f61acd7250f98d9ceeab18281b26fcabbc8845a6749814851626bacbf51
13                      50c3000000000000
14                      656ed007
recipientAddressLength  28000000
recipientAddress        5441353435494341564e45554446554249484f3343454a425356495a37595948464658354c515054
amount                  40420f0000000000
messageFieldLength      00000000
```


## Mutisig signature transaction part schema
7. Mutisig signature transaction part


```
Property                                Types                                       Description
----------------------------------------------------------------------------------------------------------------------
hashObjectLength                        uint32_t                                    Hash of the corresponding multisig transaction
hashLength                              uint32_t                                    always 0x20
hash                                    32 bytes                                    32 bytes of SHA3 hash
addressLength                           uint32_t                                    Length of address of the corresponding multisig account (always 40 = 0x28)
multisigAccountAddress                  40 bytes                                     40 bytes (using UTF8 encoding).
```

Example:

Raw transaction:
```
02100000010000989c5fd007200000003e6e6cbac488b8a44bdf5abf27b9e1cc2a6f20d09d550a66b9b36f525ca222eef049020000000000ac6dd0072400000020000000d2c70f814fa87b13da000ca42e52085fa233ce0aae718aaefe16c5652d1a6932280000005443453752474f444a354d4c4d354d43564e43495253575445484d4c594545465459355442585142
```

### Parse above transaction
```
08                      02100000
09                      01000098
10                      9c5fd007
11                      20000000
12                      3e6e6cbac488b8a44bdf5abf27b9e1cc2a6f20d09d550a66b9b36f525ca222ee
13                      f049020000000000
14                      ac6dd007

hashObjectLength        24000000
hashLength              20000000
hash                    d2c70f814fa87b13da000ca42e52085fa233ce0aae718aaefe16c5652d1a6932
addressLength           28000000
multisigAccountAddress  5443453752474f444a354d4c4d354d43564e43495253575445484d4c594545465459355442585142
```
