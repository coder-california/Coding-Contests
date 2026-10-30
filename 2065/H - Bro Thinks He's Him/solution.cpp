#include <bits/stdc++.h>
using namespace std;
 
typedef long long ll;
 
const ll MOD = 998244353;
 
// Fenwick Tree (Binary Indexed Tree) for mod arithmetic.
struct Fenw {
    int n;
    vector<ll> fenw;
    Fenw(int n) : n(n), fenw(n+1, 0) {}
    void update(int i, ll delta) {
        for(; i <= n; i += i & -i) {
            fenw[i] = (fenw[i] + delta) % MOD;
            if(fenw[i] < 0) fenw[i] += MOD;
        }
    }
    ll query(int i) {
        ll res = 0;
        for(; i > 0; i -= i & -i)
            res = (res + fenw[i]) % MOD;
        return res;
    }
    ll query_range(int l, int r) {
        if(l > r) return 0;
        return (query(r) - query(l-1) + MOD) % MOD;
    }
};
 
// Main
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
 
    int t;
    cin >> t;
    // Precompute powers of 2 up to maximum possible n = 200000.
    const int MAXN = 200000;
    vector<ll> pow2(MAXN+1, 0);
    pow2[0] = 1;
    for (int i = 1; i <= MAXN; i++){
        pow2[i] = (pow2[i-1] * 2) % MOD;
    }
 
    while(t--){
        string s;
        cin >> s;
        int n = s.size();
 
        // For positions 1...n, define:
        // A[i] = 2^(i-1) and B[i] = 2^(n-i)
        vector<ll> A(n+1), B(n+1);
        for (int i = 1; i <= n; i++){
            A[i] = pow2[i-1]; 
            B[i] = pow2[n-i]; 
        }
 
        // Build two Fenwicks for A-sums (one for positions where s[i]=='0' and one for s[i]=='1')
        Fenw fenwA0(n), fenwA1(n);
        // And similarly for B-sums.
        Fenw fenwB0(n), fenwB1(n);
 
        // Initialize the BITs.
        for (int i = 1; i <= n; i++){
            if(s[i-1] == '0'){
                fenwA0.update(i, A[i]);
                fenwB0.update(i, B[i]);
            } else {
                fenwA1.update(i, A[i]);
                fenwB1.update(i, B[i]);
            }
        }
 
        // Compute initial S:
        // S = sum_{j=1}^{n} [ if s[j]=='1' then B[j]*(sum_{i<j, s[i]=='0'} A[i])
        //                   else         B[j]*(sum_{i<j, s[i]=='1'} A[i]) ]
        ll S_val = 0;
        ll prefix0 = 0, prefix1 = 0;
        for (int i = 1; i <= n; i++){
            if(s[i-1]=='0'){
                S_val = (S_val + B[i] * prefix1) % MOD;
                prefix0 = (prefix0 + A[i]) % MOD;
            } else {
                S_val = (S_val + B[i] * prefix0) % MOD;
                prefix1 = (prefix1 + A[i]) % MOD;
            }
        }
 
        // The constant part: sum of 1's for each nonempty subsequence = 2^n - 1.
        ll constantPart = (pow2[n] - 1 + MOD) % MOD;
 
        int q;
        cin >> q;
        // Process each query.
        for (int qi = 0; qi < q; qi++){
            int v;
            cin >> v;
            // old value (0 or 1) at position v and new value after flip.
            int oldVal = s[v-1] - '0';
            int newVal = 1 - oldVal;
            int delta_v = newVal - oldVal; // +1 or -1
 
            // For the term T(v) (i.e. when j=v) we have:
            // if s[v]=='1' then T(v)=B[v]*(sum_{i<v, s[i]=='0'} A[i])
            // if s[v]=='0' then T(v)=B[v]*(sum_{i<v, s[i]=='1'} A[i])
            ll prefixA0 = fenwA0.query(v-1);
            ll prefixA1 = fenwA1.query(v-1);
            ll oldTerm = (oldVal==1 ? B[v] * prefixA0 : B[v] * prefixA1) % MOD;
            ll newTerm = (newVal==1 ? B[v] * prefixA0 : B[v] * prefixA1) % MOD;
            ll deltaT = (newTerm - oldTerm + MOD) % MOD;
 
            // For every j > v the flipped position v contributes A[v] to the prefix sum.
            // If s[j]=='1', then v contributed A[v] in the sum for positions with 0’s.
            // If s[j]=='0', then v contributed A[v] in the sum for positions with 1’s.
            // Thus the net change from positions j>v is:
            ll sumB0_future = fenwB0.query_range(v+1, n); // for j>v with s[j]=='0'
            ll sumB1_future = fenwB1.query_range(v+1, n); // for j>v with s[j]=='1'
            ll deltaFuture = ( (ll) delta_v * A[v] ) % MOD;
            deltaFuture = (deltaFuture * ((sumB0_future - sumB1_future) % MOD + MOD)) % MOD;
 
            ll deltaS = (deltaT + deltaFuture) % MOD;
            S_val = (S_val + deltaS) % MOD;
            if(S_val < 0) S_val += MOD;
 
            // Update the BITs at position v.
            if(oldVal == 0){
                fenwA0.update(v, -A[v]);
                fenwB0.update(v, -B[v]);
            } else {
                fenwA1.update(v, -A[v]);
                fenwB1.update(v, -B[v]);
            }
            if(newVal == 0){
                fenwA0.update(v, A[v]);
                fenwB0.update(v, B[v]);
            } else {
                fenwA1.update(v, A[v]);
                fenwB1.update(v, B[v]);
            }
 
            // Update the string.
            s[v-1] = (newVal == 0 ? '0' : '1');
 
            ll ans = (constantPart + S_val) % MOD;
            cout << ans % MOD << (qi == q-1 ? "
" : " ");
        }
    }
    return 0;
}