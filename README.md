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

## Advanced application: Binary Segmentation with 2 segments

In addition to the `Animal` example, the `AnimalCrossing` package also demonstrates how the same polymorphic design pattern can be applied to **algorithmic code**. In particular, the package includes a minimal implementation of **binary segmentation with a single change point (two segments)**.

### Cost abstraction

Binary segmentation is implemented in `C++` via a class `BinarySegmentation`, which depends only on an abstract base class `CostBase`. The `CostBase` interface exposes a single method

```cpp
double eval(int start, int end) const;
```

which computes the cost of a segment covering indices `[start, end)`.

Crucially, `BinarySegmentation` is written entirely in terms of `CostBase`, and therefore does not depend on how the cost is implemented. This mirrors the earlier `Animal` example, where `C++` functions operate on the abstract `Animal` base class rather than concrete subclasses.

### Native `CostL2` class

The package provides a native `C++` implementation of the L2 (sum-of-squares) cost via the class `CostL2`. This class precomputes cumulative sums internally, so that each call to `$eval(start, end)` runs in constant time.

```r
library("AnimalCrossing")

x <- c(rnorm(10, 0), rnorm(10, 5))
n <- length(x)

CostL2Module_Cpp <- new(CostL2, x)
binSeg2Cps_Cpp   <- new(BinarySegmentation, CostL2Module_Cpp)

binSeg2Cps_Cpp$predict(0L, n)


```


### R-defined cost via callbacks: `RCostClass`


To allow rapid prototyping and user-defined costs, the package also provides the class `RCostClass`. This class wraps an R function of the form:

```r
function(start, end) -> numeric(1)
```

and exposes it as a `CostBase` object to C++.

Because the R function is passed as a closure, any required data or precomputation (such as prefix sums) can be embedded directly in the function’s environment.


```r

# Slower, linear-time L2 cost function
CustomCostFunc <- function(start, end) {
  seg <- x[(start + 1L):end]
  m   <- mean(seg)
  sum((seg - m)^2)
}

CostL2Module_R <- new(RCostClass, CustomCostFunc)
binSeg2Cps_R   <- new(BinarySegmentation, CostL2Module_R)
binSeg2Cps_R$predict(0L, n)

```


### Identical behaviour, different implementations

Both approaches—native C++ costs and R-defined costs—can be passed to the same `BinarySegmentation class`, without any changes to the segmentation code.

```
Change point in [0, 20) at 10 | gain = 105.475
[1] 10
```


This example illustrates the key design principle promoted by the package:

> Algorithms should depend on abstract interfaces, not concrete implementations.

By using Rcpp Modules and polymorphic base classes, the same C++ algorithm can operate seamlessly on: high-performance C++ cost functions, and flexible, user-defined R cost functions, exactly as the earlier `Animal` example allows both C++ and R-defined animals to be treated uniformly.

