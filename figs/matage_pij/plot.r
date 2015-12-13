dat <- read.table("summary_maternal_age.csv",sep=";",header=T)

str(dat)

scr <- list(z=-35,x=-60,y=-10)

pdf(file="p10.pdf")
print(wireframe(p10 ~ x2 * y2 | k, data=dat,screen=scr,default.scales=list(arrows=F)))
dev.off()
pdf(file="p11.pdf")
print(wireframe(p11 ~ x2 * y2 | k, data=dat,screen=scr,default.scales=list(arrows=F)))
dev.off()
pdf(file="p20.pdf")
print(wireframe(p20 ~ x2 * y2 | k, data=dat,screen=scr,default.scales=list(arrows=F)))
dev.off()
pdf(file="p21.pdf")
print(wireframe(p21 ~ x2 * y2 | k, data=dat,screen=scr,default.scales=list(arrows=F)))
dev.off()
