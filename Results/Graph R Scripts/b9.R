# install.packages('ggplot2')
library(ggplot2)
# install.packages('scales')
library(scales)

# install.packages('doBy')
library(doBy)

library(dplyr)

#########################
# Rate trace processing #
#########################
data = read.table("b9.txt", header=T)
data$Node = factor(data$Node)
data$FaceId <- factor(data$FaceId)
data$Kilobits <- data$Kilobytes * 8
data$Type = factor(data$Type)


# exlude irrelevant types
data = subset(data, Type %in% c("SatisfiedInterests", "InInterests"))

# combine stats from all faces
data.combined = summaryBy(. ~ Time + Node + Type, data=data, FUN=sum)

data = subset(data.combined, Node %in% c("rtr-ucla-cs"))

data = na.omit(transform(data, 
          ratio=ave(Packets.sum, Time, FUN=function(y) c(NA, 100 * tail(y, -1) / head(y, -1)))))

# graph rates on selected nodes in number of incoming interest packets
g.nodes <- ggplot(data) +
  geom_point(aes (x=Time, y=ratio), size=1) +
  geom_line(aes (x=Time, y=ratio), size=0.5) +
  ylab("Satisfaction Ratio (Percentage)") +
  facet_wrap(~ Node)

png("b9.png", width=500, height=250)
print(g.nodes)
retval <- dev.off()