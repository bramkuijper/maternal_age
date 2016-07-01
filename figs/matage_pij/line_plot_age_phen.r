source("/home/bram/R/src/bramlib.r")
library("hash")
# parameters for the graphics
line.lwd <- 0.3
lwd <- 0.3
tick.cex <- 0.4
label.cex <- 0.7
legend.cex <- 0.6
plot.tck <- -0.4
plot.lwd <- 0.5
main.contour.level = 0.05

lim <- 1.0 



heights <- c(0.1, 1,0.2)
widths <- c(0.2,1,0.1)





# all variable combinations I want to plot
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

paneltje <- function(x, y, 
                        yvars, 
                        dataset, 
                        lwds,
                        ltys,
                        cols, ...)
{
    print(yvars)
    dataset <- dataset[order(dataset$x2),]
    for (i in 1:length(yvars))
    {
        panel.xyplot(x=dataset[,"x2"],
                        y=1-dataset[,yvars[[i]]],
                        type="l",
                        lwd=lwds[[i]],
                        lty=ltys[[i]],
                        col=cols[[i]])
    }

}

block <- function(row, col, 
                    xlab="", 
                    ylab="",
                    print.xlabels=T,
                    ind.label="A",
                    label="",
                    dataset,
                    lwds,
                    ltys,
                    cols,
                    yvars)
{
    xp <- xyplot(seq(0,1,0.01) ~ seq(0,1.0,0.01),
                    xlim=c(-.05,1.05),
                    ylim=c(-.05,1.05),
                    panel=paneltje,
                    dataset=dataset,
                    yvars=yvars,
                    lwds=lwds,
                    ltys=ltys,
                    cols=cols
                    )
            
    pushViewport(viewport(layout.pos.row=row,
                            layout.pos.col=col,
                            xscale=xp$x.limits,
                            yscale=xp$y.limits
                            ))

        do.call("paneltje",trellis.panelArgs(xp,1))

        #        grid.rect(gp=gpar(lwd=lwd,fill="transparent"))
        grid.lines(x=c(0,1),y=c(0,0),gp=gpar(lwd=lwd))
        grid.lines(x=c(0,0),y=c(0,1),gp=gpar(lwd=lwd))

        grid.text(x=0.5,y=1.1,just="centre",label=label,gp=gpar(cex=legend.cex))
    upViewport()
    
    pushViewport(viewport(layout.pos.row=row+1,
                            layout.pos.col=col,
                            xscale=xp$x.limits,
                            yscale=xp$y.limits
                            ))

        single.axis(range=xp$x.limits,
                        side="top",
                        labels=print.xlabels,
                        cex=tick.cex,
                        lwd=line.lwd,
                        labelcex=label.cex,
                        tck=plot.tck,
                        distance=0.5,
                        y.text.off=0.2,
                        nsub=5,
                        text=ifelse(row+1==length(heights),xlab,"")
                        )
    upViewport()
   
   # x-axis 
    pushViewport(viewport(layout.pos.row=row,
                            layout.pos.col=col-1,
                            xscale=xp$x.limits,
                            yscale=xp$y.limits
                            ))

            expr <- ylab
        single.axis(range=xp$y.limits,
                        side="right",
                        tck=plot.tck,
                        labels=col==2,
                        lwd=line.lwd,
                        cex=tick.cex,
                        x.text.off=0.3,
                        labelcex=label.cex,
                        distance=0.5,
                        nsub=5,
                        text=ifelse(col==2,expr,""))
    upViewport()
}

dat <- read.table("summary_maternal_age_final.csv",sep=";",header=T)

dat.sub <- dat[dat$k == 0.5 & dat$y2 == -1.02,]

dat.sub <- dat.sub[order(dat.sub$x2),]

for (combi in combis)
{
    combi <- sort(combi)


init.plot(file=paste("bethedge_lineplot2_p2",combi,collapse="_",sep="_"),
                width=225,
                type="pdf",
                height=150,
            font="helvetica")

    lo <- grid.layout(
                    ncol=length(widths),
                    nrow=length(heights),
                    heights=heights,
                    widths=widths)

pushViewport(viewport(layout=lo))
    sub.colors <- unlist(as.list(colorlist[combi]))

    print(colorlist[combi])
    print(paste(combi,collapse="+"))

    sub.lty <- as.list(lty_list[combi])

    # use the block function to generate one graph
    block(row=2,col=2,
            xlab=expression(paste("Frequency of environment 1 patches, ",italic(f)[italic(e)[1]])),
            ylab=expression(paste("Proportion ",italic(z)[1]," offspring",sep="")),
            print.xlabels=T,
            dataset=dat.sub,
            yvars=combi,
            cols=sub.colors,
            ltys=sub.lty,
            lwds=rep(2, times=length(combi))
            )

upViewport()

exit.plot()

}
