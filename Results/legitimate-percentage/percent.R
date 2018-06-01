#!/usr/bin/env Rscript

# install.packages('ggplot2')
library(ggplot2)
# install.packages('scales')
library(scales)
# install.packages('doBy')
library(doBy)

#########################
# Rate trace processing #
#########################

rawData <- readLines("valid.txt")
options(scipen=999)

# Extract Data Data
pattern <- "Fake interests per unit: ([.0-9]*) Valid interests per unit: ([.0-9]*) Good interests per unit: ([.0-9]*)"
result <- regmatches(rawData, gregexpr(pattern, rawData))
result <- result[grep(pattern, result)]

df <- data.frame(time = numeric(length(result)),
                 percent = numeric(length(result)),
                 stringsAsFactors = FALSE)

for (i in 1:length(result)) {
  df$time[i] <- i
  numberA <- as.numeric(sub(pattern, "\\1", result[i]))
  numberB <- as.numeric(sub(pattern, "\\2", result[i]))
  numberC <- as.numeric(sub(pattern, "\\3", result[i]))
  df$percent[i] <- numberC / (numberB+numberA)
}
