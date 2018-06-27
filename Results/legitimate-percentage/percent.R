#!/usr/bin/env Rscript

rawData <- readLines("fake.txt")
options(scipen=999)

# Extract Data Data
pattern <- "Fake interests per unit: ([.0-9]*) Valid interests per unit: ([.0-9]*) Good interests per unit: ([.0-9]*)"
result <- regmatches(rawData, gregexpr(pattern, rawData))
result <- result[grep(pattern, result)]

df <- data.frame(time = numeric(length(result)),
                 percent = numeric(length(result)),
                 stringsAsFactors = FALSE)

for (i in 1:length(result)) {
  df$time[i] <- i/10
  numberA <- as.numeric(sub(pattern, "\\1", result[i]))
  numberB <- as.numeric(sub(pattern, "\\2", result[i]))
  numberC <- as.numeric(sub(pattern, "\\3", result[i]))
  df$percent[i] <- numberC / (numberB+numberA)
}

plot(df$time, df$percent, type="l",xlab="Time (second)", ylab="Legitimate Precentage")
title("Fake Interest Attack")