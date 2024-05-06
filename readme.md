Single page solution for AE9000BM.

Input from Chintamani on 26th April 2024:
```
   We checked the meter WATT/ VAR/ PF (MODBUS) converted from AE9000BM in which following software modifications required to be done:

1.       For POWER WATT/ VAR reading on MODBUS above 1000 MW/ 1000 MVAR false reading are observed like ????. reading up to 900MW/ 900MVAR are only observed on Modbus output.

2.       In set up menu Scroll Time/ Energy Reset & LED test should be removed as it is not required for this particular meter.

Kindly make above corrections.
```

#### Action taken on 06-May-2024

1. Value is sufficient in the register
2. Updated the code and sent an email to Chintamani:
```
Dear Sir,
Resolving the "????" issue is not possible to solve from here. Please check out the raw data coming during the transaction and share those values with me.
Overall it seems that it is a Modscan issue.

We have updated the code and removed Scroll time and Energy Reset. Let LED test be there for time being.
Kindly coordinate with me to collect the software. You will need to compile it at your end and test.
```

