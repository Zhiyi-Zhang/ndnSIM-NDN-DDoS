# install.packages('ggplot2')
library(ggplot2)
# install.packages('scales')
library(scales)
# install.packages('doBy')
library(doBy)

#########################
# Rate trace processing #
#########################
target = "300-200-20-bad"

data = read.table(paste(target, "txt", sep="."), header=T)
data$Node = factor(data$Node)
data$FaceId <- factor(data$FaceId)
data$Kilobits <- data$Kilobytes * 8
data$Type = factor(data$Type)

# exlude irrelevant types
data = subset(data, Type %in% c("InInterests"))

data.attacker = subset(data, Node %in% c("as1-cs-a0",
                                         "as1-cs-a1",
                                         "as1-cs-a2",
                                         "as1-cs-a3",
                                         "as1-cs-a4",
                                         "as1-math-a0",
                                         "as1-math-a1",
                                         "as1-math-a2",
                                         "as1-math-a3",
                                         "as1-math-a4",
                                         "as2-cs-a0",
                                         "as2-cs-a1",
                                         "as2-cs-a2",
                                         "as2-cs-a3",
                                         "as2-cs-a4",
                                         "as2-math-a0",
                                         "as2-math-a1",
                                         "as2-math-a2",
                                         "as2-math-a3",
                                         "as2-math-a4",
                                         "as3-cs-a0",
                                         "as3-cs-a1",
                                         "as3-cs-a2",
                                         "as3-cs-a3",
                                         "as3-cs-a4",
                                         "as4-hw-a0",
                                         "as4-hw-a1",
                                         "as4-hw-a2",
                                         "as4-hw-a3",
                                         "as4-hw-a4",
                                         "as4-sm-a0",
                                         "as4-sm-a1",
                                         "as4-sm-a2",
                                         "as4-sm-a3",
                                         "as4-sm-a4"))
data.victim = subset(data, Node %in% c("as1-cs-server"))
data.gateway = subset(data, Node %in% c("as1-cs"))

# combine stats from all faces
data = summaryBy(. ~ Time + Node + Type, data=data, FUN=sum)
data.attacker = summaryBy(. ~ Time + Type, data=data.attacker, FUN=sum)
data.victim = summaryBy(. ~ Time + Type, data=data.victim, FUN=sum)
data.gateway = summaryBy(. ~ Time + Type, data=data.gateway, FUN=sum)
data.attacker$Node = "attacker"
data.victim$Node = "victim"
data.gateway$Node = "gateway"

result = rbind(data.victim, data.attacker, data.gateway)

# graph rates on selected nodes in number of incoming interest packets
g.nodes <- ggplot(result) +
  geom_point(aes (x=Time, y=Packets.sum, color=Node), size=1) +
  geom_line(aes (x=Time, y=Packets.sum, color=Node), size=0.5) +
  ylab("Rate [Incoming Interest/s]")

png(paste(target, "png", sep="."), width=500, height=250)
print(g.nodes)
retval <- dev.off()