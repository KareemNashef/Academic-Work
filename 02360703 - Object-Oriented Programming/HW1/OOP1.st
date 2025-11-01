'From Squeak5.3 of 13 December 2023 [latest update: #19486] on 21 June 2024 at 9:07:58 pm'!
Object subclass: #Monom
	instanceVariableNames: 'exp coef'
	classVariableNames: ''
	poolDictionaries: ''
	category: 'OOP1'!

!Monom methodsFor: 'accessing' stamp: 'KN 6/21/2024 13:47'!
coef
	^coef! !

!Monom methodsFor: 'accessing' stamp: 'KN 6/21/2024 14:17'!
derivative
	| der |
	der := Monom new.
	
	"If the exp is 0 then the derivative is 0"
	exp == 0
	ifTrue:
		[
			^der.
		].
	
	"Otherwise we derive"
	der coef:(coef * exp).
	der exp:(exp - 1).
	
	^der.! !

!Monom methodsFor: 'accessing' stamp: 'KN 6/21/2024 13:47'!
exp
	^exp.! !


!Monom methodsFor: 'adjusting' stamp: 'KN 6/21/2024 14:15'!
coef: anInteger

	"Checking if the input is an integer"
	anInteger isInteger
	ifFalse: 
		[
			self error: 'invalid input'.
		].
	
	
	coef := anInteger.! !

!Monom methodsFor: 'adjusting' stamp: 'KN 6/21/2024 14:13'!
exp: anInteger

	"Checking if the input is an integer"
	anInteger isInteger
	ifFalse: 
		[
			self error: 'invalid input'.
		].
	
	"Check if the input is negative"
	anInteger < 0
	ifTrue:
		[
			self error: 'invalid input'.
		].
	
	exp := anInteger.! !


!Monom methodsFor: 'initialize-release' stamp: 'KN 6/21/2024 13:47'!
initialize
	exp :=0.
	coef :=0.! !

'From Squeak5.3 of 13 December 2023 [latest update: #19486] on 21 June 2024 at 9:07:52 pm'!
Object subclass: #Polynom
	instanceVariableNames: 'monoms'
	classVariableNames: ''
	poolDictionaries: ''
	category: 'OOP1'!

!Polynom methodsFor: 'adjusting' stamp: 'KN 6/21/2024 18:37'!
addMonom: aMonom
	|prevVal newExp newCoef|
	"Check if the monom is valid"
	(aMonom isKindOf: Monom)
	ifFalse:
		[
			self error: 'invalid input'.
		].
	
	"Adding the monom to the dictionary"
	newExp := aMonom exp.
	newCoef := aMonom coef.
	prevVal := monoms at: newExp ifAbsent: 0.
	monoms add: newExp->(prevVal + newCoef).
	
	"Removing the zeroes from the polynom"
	monoms keysAndValuesDo: [:aKey :aValue | 
		aValue = 0
		ifTrue: [monoms removeKey:aKey].
		].! !

!Polynom methodsFor: 'adjusting' stamp: 'KN 6/21/2024 18:36'!
multiplyByMonom: aMonom
	|newDict multExp multCoef|
	"Check if the monom is valid"
	(aMonom isKindOf: Monom)
	ifFalse:
		[
			self error: 'invalid input'.
		].
	
	"Adding every entry multiplied entry into a new dictionary"
	newDict := Dictionary new.
	multExp := aMonom exp.
	multCoef := aMonom coef.
	monoms keysAndValuesDo: [:aKey :aValue | newDict add: (aKey + multExp)->(aValue * multCoef).].
	
	"Setting the new dictionary as monoms"
	monoms := newDict.
	
	"Removing the zeroes from the polynom"
	monoms keysAndValuesDo: [:aKey :aValue | 
		aValue = 0
		ifTrue: [monoms removeKey:aKey].
		].
	! !


!Polynom methodsFor: 'accessing' stamp: 'KN 6/21/2024 15:46'!
add: aPolynom
	| newPol tempMon |
	"Check if the polynom is valid"
	(aPolynom isKindOf: Polynom)
	ifFalse:
		[
			self error: 'invalid input'.
		].
	
	"Iterating over both polynoms and adding their monoms to the new polynom"
	newPol := Polynom new.
	tempMon := Monom new.
	
	monoms keysAndValuesDo: [:aKey :aValue | 
		tempMon exp: aKey.
		tempMon coef: aValue.
		newPol addMonom: tempMon.
		].
	
	(aPolynom asDictionary) keysAndValuesDo: [:aKey :aValue | 
		tempMon exp: aKey.
		tempMon coef: aValue.
		newPol addMonom: tempMon.
		].
	
	^newPol.! !

!Polynom methodsFor: 'accessing' stamp: 'KN 6/21/2024 14:31'!
asDictionary
	^monoms.! !

!Polynom methodsFor: 'accessing' stamp: 'KN 6/21/2024 18:31'!
derivative
	| derPoly tempMon |
	"Deriving each entry and adding it to the new dictionary"
	derPoly := Polynom new.
	tempMon := Monom new.
	monoms keysAndValuesDo: [:aKey :aValue | 
		(aKey - 1) < 0
		ifFalse: 
			[
			tempMon exp: (aKey - 1).
			tempMon coef: (aValue * aKey).
			derPoly addMonom: tempMon.			
			].

		].
	
	^derPoly.! !

!Polynom methodsFor: 'accessing' stamp: 'KN 6/21/2024 15:37'!
eval: anInteger
	| totalValue |
	"Checking if the input is an integer"
	anInteger isInteger
	ifFalse: 
		[
			self error: 'invalid input'.
		].
	
	"Putting the integer in the polynom"
	totalValue := 0.
	monoms keysAndValuesDo: [:aKey :aValue | totalValue := totalValue + ((anInteger raisedToInteger: aKey) * (aValue))].
	
	^totalValue.! !


!Polynom methodsFor: 'initialize-release' stamp: 'KN 6/21/2024 14:21'!
initialize
	monoms := Dictionary new.! !

'From Squeak5.3 of 13 December 2023 [latest update: #19486] on 21 June 2024 at 9:07:55 pm'!
Object subclass: #PolyStream
	instanceVariableNames: 'block'
	classVariableNames: ''
	poolDictionaries: ''
	category: 'OOP1'!

!PolyStream methodsFor: 'adjusting' stamp: 'KN 6/21/2024 16:41'!
add: aPolyStream

	"Checking if the input is valid"
	(aPolyStream isKindOf: PolyStream)
	ifFalse: [self error: 'invalid input'.].
	
	block value:2 value:aPolyStream value:0.! !

!PolyStream methodsFor: 'adjusting' stamp: 'KN 6/21/2024 16:39'!
addCoef: coef withExp: exp

	"Checking if the input is valid"
	exp isInteger
	ifFalse: [self error: 'invalid input'.].
	exp < 0
	ifTrue: [self error: 'invalid input'.].
	coef isInteger
	ifFalse: [self error: 'invalid input'.].
			
	block value:1 value:exp value:coef.! !

!PolyStream methodsFor: 'adjusting' stamp: 'KN 6/21/2024 17:58'!
filter: aSet
	block value:5 value:aSet value:0.! !

!PolyStream methodsFor: 'adjusting' stamp: 'KN 6/21/2024 17:54'!
multiplyBy: anInteger

	"Checking if the input is valid"
	anInteger isInteger
	ifFalse: [self error: 'invalid input'.].
	
	block value:4 value:anInteger value:0.! !

!PolyStream methodsFor: 'adjusting' stamp: 'KN 6/21/2024 20:12'!
substitute: anInteger

	"Checking if the input is valid"
	anInteger isInteger
	ifFalse: [self error: 'invalid input'.].
	
	anInteger < 1
	ifTrue: [self error: 'invalid input'.].
		
	block value:3 value:anInteger value:0.! !


!PolyStream methodsFor: 'accessing' stamp: 'KN 6/21/2024 16:38'!
block
	^block.! !

!PolyStream methodsFor: 'accessing' stamp: 'KN 6/21/2024 20:15'!
eval: anInteger
	"Checking if the input is valid"
	anInteger isInteger
	ifFalse: [self error: 'invalid input'.].
	
	^(block value:6 value:anInteger value:{}).! !


!PolyStream methodsFor: 'initialize-release' stamp: 'KN 6/21/2024 20:51'!
initialize
	| mainList addedList subList subTime multList multTime filterList filterTime |
	
	"Holds the monoms added by addCoef/Exp"
	mainList := LinkedList new.
	
	"Holds the polystreams added by add"
	addedList := LinkedList new.
	
	"The substitution values and the time of their creation"
	subList := LinkedList new.
	subTime := 1.
	
	"Holds the multiplication values and the time of their creation"
	multList := LinkedList new.
	multTime := 1.
	
	"Holds the filters sets and the time of their creation"
	filterList := LinkedList new.
	filterTime := 1.
	
	block := [ :whatDo :val1 :val2 |
		
		"Checking the value of whatDo then doing the needed function"
		
		"Add monom"
		whatDo = 1
		ifTrue:
			[
			"Adding the monom to the main list"
			mainList add: {val1. val2. subTime. multTime. filterTime}.
			].
		
		"Add polynom"
		whatDo = 2
		ifTrue:
			[
			"Adding the PolyStream to the added list"
			addedList add: {val1. subTime. multTime. filterTime}.
			].
		
		"Substitute x"
		whatDo = 3
		ifTrue:
			[
			"Adding the value of substitute to the list and adjusting the time"
			subList add: val1.
			subTime := subTime + 1.
			].
		
		"Multiply"
		whatDo = 4
		ifTrue:
			[
			"Adding the value of multiplier to the list and adjusting the time"
			multList add: val1.
			multTime := multTime + 1.
			].
		
		"Filter"
		whatDo = 5
		ifTrue:
			[
			"Copying the input values to a new set (operation is O(1) as by the question rules)"
			| copySet |
			copySet := val1 copy.
			filterList add: copySet.
			filterTime := filterTime + 1.
			].
		
		"Evaluate"
		whatDo = 6
		ifTrue:
			[ | totalSum currentExp currentCoef currentSubTime currentMultTime currentFilterTime temp currentStream currentFilters |
			"Calculating the main list"
			totalSum := 0.
			mainList do: [ :current|
					"Getting the monom data from the list"
					currentExp := current at: 1.
					currentCoef := current at: 2.
					currentSubTime := current at: 3.
					currentMultTime := current at: 4.
					currentFilterTime := current at: 5.
					
					"Calculating the value"
					
					"x*sub1*sub2*..."
					temp := val1.
					(currentSubTime to: subList size) do: [ :i | temp := temp * (subList at: i ifAbsent: 1)].
					
					"x^exp"
					temp := temp raisedToInteger: currentExp.
					
					"x*coef"
					temp := temp * currentCoef.
					
					"x*mult"
					(currentMultTime to: multList size) do: [ :i | temp := temp * (multList at: i ifAbsent: 1)].
					
					"Checking the filter"
					(currentFilterTime to: filterList size) do: [ :i | ((filterList at:i) includes: currentExp) ifTrue:[temp := 0].].
					
					"Checking the secondary filter"
					val2 do: [ :filter | filter = currentExp ifTrue:[temp := 0]. ].
					
					"Adding to the total"
					totalSum := totalSum + temp.
				].
			
			"Calculating the added polystreams"
			addedList do: [ :current |
					"Calculate the adjusted evaluation value"
					currentStream := current at: 1.
					currentSubTime := current at: 2.
					currentMultTime := current at: 3.
					currentFilterTime := current at: 4.
				
					"x*sub1*sub2*..."
					temp := val1.
					(currentSubTime to: subList size) do: [ :i | temp := temp * (subList at: i ifAbsent: 1)].
					
					"Getting the filters"
					currentFilters := Set new.
					(currentFilterTime to: filterList size) do: [ :i |(filterList at:i) do: [ :j | currentFilters add: j].].

					"Stream's polynoms"
					temp := currentStream block value:6 value:temp value: currentFilters.
				
					"x*mult"
					(currentMultTime to: multList size) do: [ :i | temp := temp * (multList at: i ifAbsent: 1)].
					
					"Adding to the total"
					totalSum := totalSum + temp.

				].
			
			"Return the total sum"
			totalSum.
			].
		].! !
