# install.packages('ggplot2')
library(ggplot2)
# install.packages('scales')
library(scales)

# install.packages('doBy')
library(doBy)

#########################
# Rate trace processing #
#########################
data = read.table("b2.txt", header=T)
data$Node = factor(data$Node)
data$FaceId <- factor(data$FaceId)
data$Kilobits <- data$Kilobytes * 8
data$Type = factor(data$Type)

# exlude irrelevant types
data = subset(data, Type %in% c("InInterests"))

# combine stats from all faces
data.combined = summaryBy(. ~ Time + Node + Type, data=data, FUN=sum)

data = subset(data.combined, Node %in% c("rtr-ucla-cs", "ucla-cs-webserver0-index"))

# graph rates on selected nodes in number of incoming interest packets
g.nodes <- ggplot(data) +
  geom_point(aes (x=Time, y=Packets.sum, color=Type), size=1) +
  geom_line(aes (x=Time, y=Packets.sum, color=Type), size=0.5) +
  ylab("Rate [Incoming Interest/s]") +
  facet_wrap(~ Node)

png("b2.png", width=500, height=250)
print(g.nodes)
retval <- dev.off()