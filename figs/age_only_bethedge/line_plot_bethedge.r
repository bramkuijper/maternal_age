source("/home/bram/R/src/bramlib.r")

# heights and widths of the panels that display the graphs
heights <- c(0.2, 1,0.3)
widths <- c(0.3,1,0.1,1,1.0)

# parameters for the graphics
line.lwd <- 0.3
lwd <- 0.3
tick.cex <- 0.4
label.cex <- 0.7
legend.cex <- 0.5
plot.tck <- -0.4
plot.lwd <- 0.5
main.contour.level = 0.05


# plot the desired variables
panel.opt <- function(x, y, dataset, yvars, colors, ltys, lwds,...)
{

    dataset$x1 <- 1 - dataset$x2
    dataset <- dataset[order(dataset$x1),]

    #grid.rect(

    for (i in 1:length(yvars))
    {
        panel.xyplot(x=dataset$x1,
                        y=dataset[,yvars[[i]]],
                        type="l",
                        col=colors[[i]],
                        lty=ltys[[i]],
                        lwd=lwds[[i]]
                    )
    }
}

block <- function(row, col, 
                    xlab="", ylab="",
                    print.xlabels=T,
                    ind.label="A",
                    label="",
                    dataset,
                    yvars,
                    ltys,
                    lwds,
                    colors
                    )
{
    xp <- xyplot(seq(0,1,0.01) ~ seq(0,1.0,0.01),
                    xlim=c(-.05,1.05),
                    ylim=c(-.05,1.05),
                    panel=panel.opt,
                    dataset=dataset,
                    yvars=yvars,
                    colors=colors,
                    lwds=lwds,
                    ltys=ltys
                    )
            
    pushViewport(viewport(layout.pos.row=row,
                            layout.pos.col=col,
                            xscale=xp$x.limits,
                            yscale=xp$y.limits
                            ))

        do.call("panel.opt",trellis.panelArgs(xp,1))

        #        grid.rect(gp=gpar(lwd=lwd,fill="transparent"))
        grid.lines(x=c(0,1),y=c(0,0),gp=gpar(lwd=lwd))
        grid.lines(x=c(0,0),y=c(0,1),gp=gpar(lwd=lwd))

        grid.text(x=0.5,y=1.1,just="centre",label=label,gp=gpar(cex=legend.cex))
        grid.text(x=0.1,y=0.95,just="left",label=ind.label,gp=gpar(cex=label.cex))

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
                        y.text.off=0.5,
                        nsub=5,
                        text=ifelse(row==length(heights),xlab,"")
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

dat <- read.table("summary_maternal_age_pbethedge_total.csv",sep=";",header=T)


init.plot(file="bethedge_lineplot",
                width=400,
                type="pdf",
                height=180,
            font="helvetica")

lo <- grid.layout(
                    ncol=length(widths),
                    nrow=length(heights),
                    heights=heights,
                    widths=widths)


pushViewport(viewport(layout=lo))

    
dat.sub <- dat[dat$k == 0.5 & dat$y2 == 0.14,]

    block(row=2, 
            col=2, 
            xlab="",
            ylab=expression(paste("Proportion ",italic(z)[1]," offspring")),
            print.xlabels=T,
            ind.label="A",
            label="Rapidly changing environment",
            dataset=dat.sub,
            yvars=c("p10","p11"),
            ltys=list(1,"22"),
            lwds=rep(plot.lwd,times=2),
            colors=c("red","blue")
            )
    
dat.sub <- dat[dat$k == 0.5 & dat$y2 == -0.98,]

    block(row=2, 
            col=4, 
            xlab="",
            ylab=expression(paste("Proportion ",italic(z)[1]," offspring")),
            print.xlabels=T,
            ind.label="B",
            label="Slowly changing environment",
            dataset=dat.sub,
            yvars=c("p10","p11"),
            ltys=list(1,"22"),
            lwds=rep(plot.lwd,times=2),
            colors=c("red","blue")
            )

    grid.text(x=0.4,
                y=0.08,
                label=expression(paste("Frequency environment 1")),
                gp=gpar(cex=label.cex))
                
    # legend
    pushViewport(viewport(layout.pos.row=2,
                            layout.pos.col=5))


        y.pos <- 0.9
        grid.lines(x=c(0.1,0.2),
                    y=c(y.pos,y.pos),
                    gp=gpar(col="blue",
                            lwd=plot.lwd,
                            lineend="square",
                            lty="22"))
        grid.text(x=0.25,
                    y=y.pos,
                    just="left",
                    label=expression(paste("Young mothers, ", italic(p)["young"])),
                    gp=gpar(cex=legend.cex))


        y.pos <- 0.8
        grid.lines(x=c(0.1,0.2),
                    y=c(y.pos,y.pos),
                    gp=gpar(col="red",
                            lwd=plot.lwd,
                            lineend="square",
                            lty=1))
        grid.text(x=0.25,
                    y=y.pos,
                    just="left",
                    label=expression(paste("Old mothers, ", italic(p)["old"])),
                    gp=gpar(cex=legend.cex))

    upViewport()
upViewport()

exit.plot()
