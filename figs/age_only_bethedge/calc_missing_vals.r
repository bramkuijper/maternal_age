source("/home/bram/R/src/bramlib.r")

    
dat <- read.table("summary_maternal_age_pbethedge.csv",sep=";",header=T)

#miss.vals <- missing.vals.level(dat, "x2","y2")

dataset <- dat

x <- "x2"
y <- "y2"
z <- "k"

# unique x,y values

# calculate the missing vals
a <- table(dat[,c(x,y,z)])
b <- as.data.frame(a)
c <- b[b$Freq == 0,]
#
c$x2 <- as.numeric(as.character(c$x2))
c$y2 <- as.numeric(as.character(c$y2))
#
## now transform these back into s1 and s2 values
c[,"s2"] <- sqrt(((1.0 - c$x2)/c$x2) * 10^(2*c$y2))
c[,"s1"] <- 10^(2*c$y2)/c$s2
#

#
f <- file("newruns.sh","w")

for (i in 1:nrow(c))
{

    simruns_line <- paste("/home/uccoaku/maternal_age/numsolve 1000000 0.5 0.5 0.5 0.5 0 0 ", c[i,"k"]," 0.5 ",c[i,"s1"], " ", c[i,"s2"], " 0.05 0.5 0.5 0.125 0.125 0.125 0.125 0.125 0.125 0.125 0.125 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5")

    writeLines(text=simruns_line,con= f)
}
close(f)
