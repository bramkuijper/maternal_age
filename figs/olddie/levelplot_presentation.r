library("lattice")
library("colorRamps")

source("/home/bram/R/src/bramlib.r")

data <- read.table("summary_maternal_age_olddie.csv",sep=";",header=T)

type <- "svg"

init.plot("levelplot_diff_pij",
        width=400,
        height=400,
        type=type,
        background="transparent",
        font="myriad"
        )

print(levelplot(p11 - p10 ~ x2 * y2,
            data=data[data$k==0.5,],
            xlab="",
            ylab="",
            col.regions=c(rev(blue2yellow(50)),matlab.like(50))
            ))

exit.plot()





init.plot("levelplot_diff_pij2",
        width=400,
        height=400,
        type=type,
        background="transparent",
        font="myriad"
        )

print(levelplot(p21 - p20 ~ x2 * y2,
            data=data[data$k==0.5,],
            xlab="",
            ylab="",
            col.regions=c(rev(blue2yellow(50)),matlab.like(50))
            ))

exit.plot()

