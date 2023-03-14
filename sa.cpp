//  piekne SA Psyho.

// Ustawias T0 - czyli poczatkowa temperature
// Ustawiasz TN - czyli koncowa temperature
// i ustawiasz limit czasu
// I NAPIERDALASZZZZZZZZZZZZ
 
 int hc_step = 0;
    double sa_t0 = 0.5;
    double sa_tn = 1e-6;
    double sa_t = sa_t0;
    int lowest_accept = 0;
    while (true) {
        if ((hc_step & 31) == 0) {
            time_passed = (elapsed() - TIME_LIMIT_RANDOM) / (TIME_LIMIT_HC - TIME_LIMIT_RANDOM);
            sa_t = sa_t0 * pow(sa_tn / sa_t0, time_passed);
        }
        if (time_passed > 1.0) break;

        int av = sim<false>(targets, meta);
        if (av >= bv || rng.next_double() < exp((av - bv) / sa_t)) {
            if (av > xv) {
                DB(hc_step, elapsed(), av);
                xv = av;
                xtargets = targets;
                xmeta = meta;
            }
            lowest_accept = max(lowest_accept, bv - av);
            bv = av;
            btargets = targets;
            bmeta = xmeta;
        }
    }