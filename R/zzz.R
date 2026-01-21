#' @useDynLib AnimalCrossing, .registration = TRUE
#' @importFrom Rcpp loadModule
#' @import methods
#' @export


.onLoad <- function(libname, pkgname) {
  Rcpp::loadModule("animal_module", TRUE)
}

