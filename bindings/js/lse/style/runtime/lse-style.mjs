import{__internal as t}from'@lse/core';const n=(t,n,e)=>o(t,n,e,255);const o=(t,n,o,e)=>(l(e,0,255)|l(o,0,255)<<8|l(n,0,255)<<16|l(t,0,255)<<24)>>>0;const e=(t,n,o)=>s(t,n,o,255);const s=(t,e,s,r)=>{e=l(e,0,1);s=l(s,0,1);if(e===0){const t=Math.round(s*255);return n(t,t,t)}t=l(t,0,360)/360;const i=s<.5?s*(1+e):s+e-s*e;const S=2*s-i;return o(c(S,i,t+1/3),c(S,i,t),c(S,i,t-1/3),r)};const c=(t,n,o)=>{let e;if(o<0){o+=1}if(o>1){o-=1}if(o<1/6){e=t+(n-t)*6*o}else if(o<1/2){e=n}else if(o<2/3){e=t+(n-t)*(2/3-o)*6}else{e=t}return Math.round(e*255)};const l=(t,n,o)=>t>o?o:t<n?n:t;const{StyleTransform:r}=t.Style;const{IDENTITY:i,TRANSLATE:S,SCALE:f,ROTATE:y}=r;const T=()=>new r(i);const a=(t,n)=>new r(S,t,n);const w=t=>a(t,0);const u=t=>a(0,t);const I=(t,n)=>new r(f,t,n);const h=t=>I(t,1);const m=t=>I(1,t);const p=t=>new r(y,t);const{StyleFilter:A}=t.Style;const{FLIP_H:E,FLIP_V:L,TINT:b}=A;const F=()=>new A(E);const N=()=>new A(L);const d=t=>new A(b,t);const{StyleValue:g}=t.Style;const C=(t,n)=>new g(t,n);const M=Object.freeze({position:'absolute',left:0,top:0,right:0,bottom:0});const{assign:O,flatten:P,StyleSheet:R,StyleClass:V}=t.Style;const _='2.0.0';export{V as StyleClass,R as StyleSheet,M as absoluteFill,O as assign,P as flatten,F as flipH,N as flipV,e as hsl,s as hsla,T as identity,n as rgb,o as rgba,p as rotate,I as scale,h as scaleX,m as scaleY,d as tint,a as translate,w as translateX,u as translateY,C as value,_ as version};
