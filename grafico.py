import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

r2p, devp, triesp, tries_devp, passip = np.loadtxt(r'stats_salvate\Pivot400_1e5.txt', delimiter=',', unpack=True, skiprows=1)
r2ss, devss, triesss, tries_devss, passiss = np.loadtxt(r'stats_salvate\SimpleSampling50_1e6.txt', delimiter=',', unpack=True, skiprows=1)
#r2p, devp, passip = np.loadtxt('Dimerizzazione.txt', delimiter=',', unpack=True, skiprows=1)

passip_b, c_N, R_e2, R_g, R_m = np.loadtxt('exact_enumeration.txt', delimiter=None, unpack=True) 
R_e2 = (4/c_N)*R_e2


def expo(N, Aa, nu):
    return Aa * N**(2*nu)

[popt_expo, pcov_expo] = curve_fit(expo, passip, r2p, p0=(1, 0.75), sigma=devp, absolute_sigma=True)
Aa = popt_expo[0]
nu = popt_expo[1]
err = np.sqrt(np.diag(pcov_expo))

#analisi fit
res_expo = r2p - expo(passip, Aa, nu)
res_expo = res_expo / devp
chisq_expo = (res_expo**2).sum()
chisq_expo_dof = chisq_expo / (len(r2p)-2)

print("parametri: Aa=", Aa," pm ",err[0], "   nu=", nu, " pm ", err[1], "chisq= ", chisq_expo_dof)
#print("parametri efficienza: Cc=", Cc," pm ",err_att[0], " lam= ", lam, " pm ", err_att[1], " gam= ", gam, " pm ", err_att[2],  "chisq= ", chisq_att_dof)

#plot
xx = np.linspace(0, max(passip), 1000)
fit_eq = (
    r"$<R^2> \sim A N^{2\nu}$" "\n"
    + r"$A = {:.4f} \pm {:.4f}$".format(Aa, err[0]) + "\n"
    + r"$\nu = {:.4f} \pm {:.4f}$".format(nu, err[1]) +"\n"
    + r"$\chi_r^2 = {:.4f}$".format(chisq_expo_dof)
)



fig1, axs = plt.subplots(nrows=2, ncols=1, sharex=True, gridspec_kw={'height_ratios': [3, 1]})
axs[0].set_title("Distanza end-to-end di una catena di lunghezza N")
axs[0].errorbar(passip, r2p, devp, fmt='.')
axs[0].plot(xx, expo(xx, Aa,  nu))
axs[0].set_ylabel('$<R^2>$')
axs[0].text(1,88, fit_eq)


axs[1].errorbar(passip[passip >= 2], res_expo, 1, fmt='.') 
axs[1].axhline(0, color='gray', linestyle='--')
axs[1].set_xlabel('Numero di passip')
axs[1].set_ylabel('Sigma')



#plt.figure()
#plt.plot(passip[passip <= 59], (r2p[passip <= 59]- R_e2[passip_b >= 5]))
plt.show()



'''
#acceptance fraction
ff=1/triesp
ff_devp= triesp_devp/(triesp**2)

def acceptance_fraction(N, B, p):
    return B*np.exp(-p*N)

[poptAcc, pcovAcc] = curve_fit(acceptance_fraction, passip, ff*1e4, p0=(1, 0.19), sigma=ff_devp*1e4, absolute_sigma=True)
Bb= poptAcc[0]
p = poptAcc[1]
errAcc = np.sqrt(np.diag(pcovAcc))
print("parametri: Bb=", Bb," pm ",errAcc[0], "   p=", p, " pm ", errAcc[1])


plt.figure()
plt.title("Acceptance Fraction")
plt.errorbar(passip, ff*1e4, ff_devp*1e4, fmt='.')
plt.plot(passip, acceptance_fraction(passip, Bb, p), color='red')
plt.plot(passip, 1*np.exp(-0.19*passip), color='green')
plt.xlabel('Numero di passip')
plt.ylabel('Frazione di accettazione')

#plt.figure()
#plt.scatter(passip, devp/r2p, marker='.', color='red')
#plt.xlabel('Numero di passip')
#plt.ylabel(r'$\Delta <R^2>$')

'''