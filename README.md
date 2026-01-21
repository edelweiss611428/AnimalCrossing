# AnimalCrossing

AnimalCrossing is a sample R package demonstrating how C++ classes defined using Rcpp Modules can be exposed to R and used polymorphically from both C++ and R. The package shows how multiple C++ classes deriving from a common base class can be passed to C++ functions through a shared interface, and how objects defined in R can be mapped to the same C++ base class via callback methods.

This package is intended for demonstration and educational purposes only.

## Description

The package defines a C++ base class `Animal`. An `Animal` object is defined by three arguments:

- `type`: a string describing the animal type (for example `"duck"`, `"dog"`, or any other user-defined type);
- `name`: the name of the animal;
- `age`: the age of the animal.

Two native C++ subclasses, `Duck` and `Dog`, are provided. These subclasses automatically set the `type` argument (to `"duck"` or `"dog"`) and require only `name` and `age` when constructed in R.

In addition, the package provides a special C++ class `RAnimal`, which allows users to define new animals directly in R by supplying callback functions for `type`, `name`, and `age`. These R-defined objects can be passed to C++ functions anywhere an `Animal` object is expected.

## Sample usage

The following examples show both native C++ animals and R-defined animals being passed to the same C++ function.

```r
library("AnimalCrossing")
duck <- new(Duck, "Aquaduck", 10)
print_animal(duck)
```
```
Aquaduck is a duck and they are 1 year old
```

```r
moth <- new(
  RAnimal,
  type = function() "moth",
  name = function() "Connie",
  age  = function() 1L
)

print_animal(moth)
```

```
Connie is a moth and they are 1 year old
```
