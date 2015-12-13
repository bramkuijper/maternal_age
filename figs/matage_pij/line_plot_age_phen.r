source("/home/bram/R/src/bramlib.r")
library("hash")
dat <- read.table("summary_maternal_age.csv",sep=";",header=T)

dat.sub <- dat[dat$k == 0.5 & dat$y2 == -1.02,]

dat.sub <- dat.sub[order(dat.sub$x2),]

combis <- list(c1=c("p10","p20"),
                c2=c("p10"),
                c3=c("p11"),
                cx=c("p20","p21"),
                c4=c("p10","p11"),
                c5=c("p11","p21"),
                c6=c("p11","p21","p10","p20")
                )

colorlist=hash(values=c("red","blue","#ff6913","#ff13fa"),
                        keys=c("p11","p10","p21","p20"))

lty_list =hash(values=c(2,1,2,1),
                        keys=c("p11","p10","p21","p20"))
for (combi in combis)
{
    combi <- sort(combi)

init.plot(file=paste("bethedge_lineplot2",combi,collapse="_",sep="_"),
                width=500,
                type="svg",
                height=400,
            font="myriad")

    sub.colors <- unlist(as.list(colorlist[combi]))

    print(colorlist[combi])
    print(paste(combi,collapse="+"))

    sub.lty <- unlist(as.list(lty_list[combi]))

print(xyplot(as.formula(paste(paste(combi,collapse="+"),"~x2")),
                data=dat.sub,
                type="l",
                panel=function(...) { panel.abline(v=0.5,col="grey50",lty=3); panel.xyplot(...); },
                #                auto.key=T,
                lwd=2.0,
                lty=sub.lty,
                xlab="",
                ylab="",
                scales=list(x=list(tck=c(1,0)),
                            y=list(tck=c(1,0))),
                    col=sub.colors
                    #                    col=c("transparent","blue","transparent","transparent")
                    #col=c("transparent","blue","transparent","#ff13fa")
                #                col=c("red","blue",,"#ff6913","#ff13fa")
                ))

exit.plot()

}
