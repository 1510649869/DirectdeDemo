#pragma once  
#ifndef HR  
#define HR(x)    { hr = x; if( FAILED(hr) ) { return hr; } }         //�Զ���һ��HR�꣬����ִ�д���ķ���  
#endif  

#ifndef SAFE_DELETE                   
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }       //�Զ���һ��SAFE_RELEASE()��,����ָ����Դ���ͷ�  
#endif      

#ifndef SAFE_RELEASE              
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }     //�Զ���һ��SAFE_RELEASE()��,����COM��Դ���ͷ�  
#endif  

#pragma once
#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600
#define WINDOW_CLASS L"WndClass"
#define WINDOW_TITLE L"DirectWnd"

