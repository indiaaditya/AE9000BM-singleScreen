24/11/22
Added VAR and WATT limit defines

#define KW_POSITIVE_SET 0.8
#define KW_NEGATIVE_SET -0.8
#define VAR_POSITIVE_SET 0.8
#define VAR_NEGATIVE_SET -0.8


#define KW_TOT_POSITIVE_SET 2.5
#define KW_TOT_NEGATIVE_SET -2.5
#define VAR_TOT_POSITIVE_SET 2.5
#define VAR_TOT_NEGATIVE_SET -2.5


Tested OK by Atul and Chintamani.



24/11/22
Offset issue resolution
Problem faced: Current at zero shows offset value. Initial offset was set to 10 mA. But still the values were seen. 
Increased the value to 35 but still the problem is observed.

Observation:
For Low Current,
current is calculated by VA/V.
It is observed that VA is 0.

VA is 0 because KWr is 0.

KWr is 0 because of limits set of -1.6 to +1.6. 


06/12/2022
When input was switched off suddenly, the meter used to hang specifically in the VAR screen.
Hard Fault was being detected.
Modified the calculation.c file.

In Para_Store function:
Added lclIndex1, lclIndex2
The buffer array variable has been filled with a complex calculation.
Added a layer of verification to ensure that no wrong index is sent to the index.

