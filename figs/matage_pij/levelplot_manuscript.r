library("lattice")
library("colorRamps")

source("/home/bram/R/src/bramlib.r")

data <- read.table("summary_maternal_age_final.csv",sep=";",header=T)

type <- "svg"

init.plot("levelplot_diff_pij_ms",
        width=400,
        height=400,
        type=type,
        background="transparent",
        font="helvetica"
        )


print(levelplot(.5 * ( p11 - p10 + p20 - p21) ~ x2 * y2,
            data=data[data$k==0.5,],
            xlab="",
            ylab="",
            col.regions=matlab.like(50)
            ))

exit.plot()
