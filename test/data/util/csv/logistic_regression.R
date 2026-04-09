#!/usr/bin/Rscript

data <- read.csv( file="logistic_regression.csv", sep=",", head=TRUE )

x <- data[,'Hours']
y <- data[,'Pass']

# # Normal
# res <- lm(formula = y ~ x, model=TRUE, x=TRUE, y=TRUE, qr=TRUE )
#
# # No intercept
# # res <- lm(formula = y ~ x - 1, model=TRUE, x=TRUE, y=TRUE, qr=TRUE )
#
# anova(res)
# summary(res)
# print(res)
#
#
# print("resid")
# paste0( resid(res), collapse=", " )
# # residuals(res)
# # paste0( res['residuals'], collapse=", " )
# cat("\n")
#
# print("fitted")
# # paste0( fitted(res), collapse=", " )
# cat("\n")
#
# print("fitted.values")
# paste0( res['fitted.values'], collapse=", " )
# cat("\n")
#
# print("x")
# # res['x']
# paste0( res['x'], collapse=", " )
# cat("\n")
#
# # print("qr")
# # paste0( res['qr'], collapse=", " )
#
# print( res['weights'] )
# print( res['coefficients'] )
# print( res['xlevels'] )
# print( res['rank'] )
# print( res['contrasts'] )
# # print( res['model'] )
# print( res['df.residual'] )


fit <- glm( formula = y ~ x, family=binomial(link='logit') )
summary(fit) # display results
confint(fit) # 95% CI for the coefficients
exp(coef(fit)) # exponentiated coefficients
exp(confint(fit)) # 95% CI for exponentiated coefficients
predict(fit, type="response") # predicted values
residuals(fit, type="deviance") # residuals
