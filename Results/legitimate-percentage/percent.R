#!/usr/bin/env Rscript

library(ggplot2)

args = commandArgs(trailingOnly=TRUE)
#target = args[1]

target = "two-valid-f100-c750"
rawData <- readLines(target)
options(scipen=999)

# Extract Data Data
pattern <- "Fake interests per unit: ([.0-9]*) Valid interests per unit: ([.0-9]*) Good interests per unit: ([.0-9]*)"
result <- regmatches(rawData, gregexpr(pattern, rawData))
result <- result[grep(pattern, result)]

df <- data.frame(time = numeric(length(result)),
                 percent = numeric(length(result)),
                 stringsAsFactors = FALSE)

is.even <- function(x) x %% 2 == 0
is.odd <- function(x) x %% 2 != 0

for (i in 1:length(result)) {
  df$time[i] <- i/10
  df$time[i] <- df$time[i]/2
  if (is.even(i)) {
    df$node[i] = "A"
  }
  if (is.odd(i)) {
    df$node[i] = "B"
    df$time[i] = df$time[i] + 0.05
  }
  
  numberA <- as.numeric(sub(pattern, "\\1", result[i]))
  numberB <- as.numeric(sub(pattern, "\\2", result[i]))
  numberC <- as.numeric(sub(pattern, "\\3", result[i]))
  df$percent[i] <- numberC / (numberB+numberA)
}

#plot(df$time, df$percent, type="l",xlab="Time (second)", ylab="Legitimate Precentage")
#title("Fake Interest Attack")

# graph rates on selected nodes in number of incoming interest packets
g.nodes <- ggplot(df, aes (x=time, y=percent, group=node)) +
  geom_line(size=1) +
  xlab("Time [second]") +
  ylab("Legit Traffic pct. %") +
  theme_linedraw() +
  theme(
    legend.position="none", text = element_text(size=12),
    axis.text.x = element_text(color = "grey20", size = 33, angle = 0, face = "plain"),
    axis.text.y = element_text(color = "grey20", size = 33, angle = 90, face = "plain"),
    axis.title.x = element_text(color="black", size=33, face="bold"),
    axis.title.y = element_text(color="black", size=33, face="bold")
  )

png(paste(target, "png", sep="."), width=500, height=500)
print(g.nodes)
retval <- dev.off()