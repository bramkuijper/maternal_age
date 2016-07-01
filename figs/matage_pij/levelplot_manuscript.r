library("lattice")
library("colorRamps")

source("/home/bram/R/src/bramlib.r")

data <- read.table("summary_maternal_age_final.csv",sep=";",header=T)

type <- "pdf"

the.strip <- function(which.given, which.panel, factor.levels,...) { 

    print(factor.levels)
        
    k.val <<- gsub("[\\{\\}[:space:]]","",factor.levels[[which.panel[[1]]]])
        
    the_label <- eval(substitute(
                            expression(paste("dispersal cost ",italic(k),"=",kx,sep="")),
                            list(kx=k.val)
                    ))
    llines(c(0, 1, 1, 0, 0), c(0, 0, 1, 1, 0), col="black")
    ltext(x=0.5,
            y=0.5,
            labels=the_label
            )

}

data$x1 = 1.0 - data$x2

pdf(file="levelplot_diff_p1_ms.pdf")
print(levelplot((p11 - p10) ~ x1 * y2 | k,
                data=data,
                xlab=expression(paste("Frequency of ",italic(e)[1]," patches, ",italic(f)[italic(e)[1]])),
                ylab=expression(paste("Average rate of environmental change, ",bar(italic(s)))),
                strip=the.strip,
                col.regions=matlab.like,
                colorkey=list(space="right", title="Doesn't work"), 
                main=expression(paste("Age difference in offspring phenotype determination of ",italic(z)[1]," parents: ",italic(p)[paste(italic(o),"1")],{}-{},italic(p)[paste(italic(y),"1")]))
            ))

dev.off()

pdf(file="levelplot_diff_p2_ms.pdf")
print(levelplot((p21 - p20) ~ x1 * y2 | k,
                data=data,
                xlab=expression(paste("Frequency of ",italic(e)[1]," patches, ",italic(f)[italic(e)[1]])),
                ylab=expression(paste("Average rate of environmental change, ",bar(italic(s)))),
                strip=the.strip,
                col.regions=matlab.like,
                colorkey=list(space="right", title="Doesn't work"), 
                main=expression(paste("Age difference in offspring phenotype determination of ",italic(z)[2]," parents: ",italic(p)[paste(italic(o),"1")],{}-{},italic(p)[paste(italic(y),"1")]))
            ))
dev.off()
