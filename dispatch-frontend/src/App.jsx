import { useState, useEffect } from 'react';
import api from './api';

function App() {
  const [systemState, setSystemState] = useState({
    pendingOrders: [],
    stagingOrders: [],
    activeDeliveries: [],
    availableDrivers: []
  });

  const [newOrder, setNewOrder] = useState({ id: '', customer: '', priority: 3 });
  const [error, setError] = useState(null);

  const fetchState = async () => {
    try {
      const res = await api.get('/state');
      setSystemState(res.data);
      setError(null);
    } catch (err) {
      setError("CONNECTION ERROR: Cannot reach C++ Backend on port 8080.");
    }
  };

  useEffect(() => {
    fetchState();
  }, []);

  const handleAddOrder = async (e) => {
    e.preventDefault();
    try {
      await api.post('/orders', {
        id: parseInt(newOrder.id),
        customer: newOrder.customer,
        priority: parseInt(newOrder.priority)
      });
      setNewOrder({ id: '', customer: '', priority: 3 });
      fetchState(); 
    } catch (err) {
      setError("Failed to add order to Staging Heap.");
    }
  };
  
  const handleMergeHubs = async () => {
    try {
      await api.post('/merge');
      fetchState();
    } catch (err) {
      setError("Merge operation failed.");
    }
  };

  const handleDispatch = async () => {
    try {
      await api.post('/dispatch');
      fetchState();
    } catch (err) {
      setError(err.response?.data?.error || "Dispatch failed. Check Treap/Heap status.");
    }
  };

  const handleCompleteDelivery = async (orderId) => {
    try {
      await api.post('/complete', { id: orderId });
      fetchState();
    } catch (err) {
      setError(err.response?.data?.error || "Failed to complete delivery and update AVL Tree.");
    }
  };

  return (
    <div className="min-h-screen p-4 md:p-8 font-sans">
      <div className="max-w-[1600px] mx-auto space-y-6">
        
        {/* Header Section */}
        <header className="flex flex-col md:flex-row justify-between items-end border-b border-gray-800 pb-4">
          <div>
            <h1 className="text-4xl font-bold tracking-tight text-white mb-2 uppercase">
              Dispatch <span className="text-gray-500">Terminal</span>
            </h1>
            <div className="flex gap-4 text-xs font-mono text-gray-400">
              <span className="flex items-center gap-1">
                <span className={`w-2 h-2 rounded-full ${error ? 'bg-red-500' : 'bg-emerald-500 animate-pulse'}`}></span>
                {error ? 'SYSTEM OFFLINE' : 'SYSTEM ONLINE'}
              </span>
              <span>|</span>
              <span>ENGINE: C++17</span>
              <span>|</span>
              <span>MEM_MODE: RAW_PTR</span>
            </div>
          </div>
          
          <div className="flex gap-4 mt-4 md:mt-0">
            <button 
              onClick={handleMergeHubs}
              className="bg-purple-900 text-purple-100 hover:bg-purple-800 font-bold py-3 px-6 text-sm tracking-widest uppercase transition-all active:scale-95"
            >
              Merge Hubs [M]
            </button>
            <button 
              onClick={handleDispatch}
              className="bg-white text-black hover:bg-gray-200 font-bold py-3 px-8 text-sm tracking-widest uppercase transition-all active:scale-95"
            >
              Execute Dispatch [↵]
            </button>
          </div>
        </header>

        {error && (
          <div className="bg-red-950 border border-red-500 text-red-400 p-3 font-mono text-sm uppercase">
            {">"} {error}
          </div>
        )}

        {/* Input Panel */}
        <div className="bg-[#111] border border-gray-800 p-5">
          <div className="text-xs font-mono text-gray-500 mb-3 uppercase tracking-wider">Initialize New Order (Routes to Staging) -{">"}</div>
          <form onSubmit={handleAddOrder} className="flex flex-wrap gap-4 items-end">
            <div className="flex-1 min-w-[150px]">
              <label className="block text-xs font-mono text-gray-400 mb-1">ID_HASH</label>
              <input type="number" required className="w-full bg-black border border-gray-700 text-white font-mono p-2 text-sm focus:border-white focus:outline-none transition-colors" 
                value={newOrder.id} onChange={e => setNewOrder({...newOrder, id: e.target.value})} placeholder="e.g. 1045" />
            </div>
            <div className="flex-[2] min-w-[200px]">
              <label className="block text-xs font-mono text-gray-400 mb-1">DESTINATION_STR</label>
              <input type="text" required className="w-full bg-black border border-gray-700 text-white p-2 text-sm focus:border-white focus:outline-none transition-colors" 
                value={newOrder.customer} onChange={e => setNewOrder({...newOrder, customer: e.target.value})} placeholder="Enter destination..." />
            </div>
            <div className="flex-1 min-w-[150px]">
              <label className="block text-xs font-mono text-gray-400 mb-1">PRIORITY_LVL</label>
              <select className="w-full bg-black border border-gray-700 text-white font-mono p-2 text-sm focus:border-white focus:outline-none transition-colors appearance-none"
                value={newOrder.priority} onChange={e => setNewOrder({...newOrder, priority: e.target.value})}>
                <option value={1}>[1] CRITICAL</option>
                <option value={2}>[2] STANDARD</option>
                <option value={3}>[3] ROUTINE</option>
              </select>
            </div>
            <button type="submit" className="bg-gray-800 hover:bg-gray-700 text-white border border-gray-600 font-mono text-sm py-2 px-6 h-[38px] transition-colors">
              INJECT
            </button>
          </form>
        </div>

        {/* 4-Column Data Structure Dashboard */}
        <div className="grid grid-cols-1 md:grid-cols-4 gap-4 h-[600px]">
          
          {/* Binomial Heap (Staging) */}
          <div className="bg-[#111] border border-gray-800 flex flex-col">
            <div className="p-3 border-b border-gray-800 flex justify-between items-center">
              <h2 className="text-sm font-bold text-purple-500 tracking-wider uppercase">Binomial Heap (Staging)</h2>
              <span className="text-xs font-mono text-gray-500">n = {systemState.stagingOrders.length}</span>
            </div>
            <div className="p-4 flex-1 overflow-y-auto space-y-2">
              {systemState.stagingOrders.length === 0 && <div className="text-xs font-mono text-gray-600">{">"} STAGING_EMPTY</div>}
              {systemState.stagingOrders.map(order => (
                <div key={order.id} className="group border border-gray-800 bg-black p-3 hover:border-purple-900 transition-colors">
                  <div className="flex justify-between items-start mb-1">
                    <span className="font-mono text-sm text-gray-300">ORD_{order.id}</span>
                    <span className={`text-[10px] font-mono px-1.5 border ${
                      order.priority == 1 ? 'border-red-900 text-red-500' : 
                      order.priority == 2 ? 'border-purple-900 text-purple-500' : 'border-gray-700 text-gray-500'
                    }`}>
                      P{order.priority}
                    </span>
                  </div>
                  <div className="text-xs text-gray-500 truncate group-hover:text-gray-400">{order.customer}</div>
                </div>
              ))}
            </div>
          </div>

          {/* Binomial Heap (Main) */}
          <div className="bg-[#111] border border-gray-800 flex flex-col">
            <div className="p-3 border-b border-gray-800 flex justify-between items-center">
              <h2 className="text-sm font-bold text-amber-500 tracking-wider uppercase">Binomial Heap (Main)</h2>
              <span className="text-xs font-mono text-gray-500">n = {systemState.pendingOrders.length}</span>
            </div>
            <div className="p-4 flex-1 overflow-y-auto space-y-2">
              {systemState.pendingOrders.length === 0 && <div className="text-xs font-mono text-gray-600">{">"} HEAP_EMPTY</div>}
              {systemState.pendingOrders.map(order => (
                <div key={order.id} className="group border border-gray-800 bg-black p-3 hover:border-amber-900 transition-colors">
                  <div className="flex justify-between items-start mb-1">
                    <span className="font-mono text-sm text-gray-300">ORD_{order.id}</span>
                    <span className={`text-[10px] font-mono px-1.5 border ${
                      order.priority == 1 ? 'border-red-900 text-red-500' : 
                      order.priority == 2 ? 'border-amber-900 text-amber-500' : 'border-gray-700 text-gray-500'
                    }`}>
                      P{order.priority}
                    </span>
                  </div>
                  <div className="text-xs text-gray-500 truncate group-hover:text-gray-400">{order.customer}</div>
                </div>
              ))}
            </div>
          </div>

          {/* AVL Tree */}
          <div className="bg-[#111] border border-gray-800 flex flex-col">
            <div className="p-3 border-b border-gray-800 flex justify-between items-center">
              <h2 className="text-sm font-bold text-cyan-500 tracking-wider uppercase">AVL Tree</h2>
              <span className="text-xs font-mono text-gray-500">n = {systemState.activeDeliveries.length}</span>
            </div>
            <div className="p-4 flex-1 overflow-y-auto space-y-2">
              {systemState.activeDeliveries.length === 0 && <div className="text-xs font-mono text-gray-600">{">"} TREE_EMPTY</div>}
              {systemState.activeDeliveries.map(order => (
                <div key={order.id} className="group relative border border-gray-800 bg-black p-3 border-l-2 border-l-cyan-800 hover:border-cyan-900 transition-colors">
                  <div className="flex justify-between items-center mb-2">
                    <span className="font-mono text-sm text-gray-300">ORD_{order.id}</span>
                    <span className="text-[10px] font-mono text-cyan-600 bg-cyan-950 px-1 group-hover:hidden">ACTIVE</span>
                    <button 
                      onClick={() => handleCompleteDelivery(order.id)}
                      className="hidden group-hover:block text-[10px] font-mono bg-cyan-700 hover:bg-cyan-600 text-white px-2 py-0.5 transition-colors"
                    >
                      MARK COMPLETE [X]
                    </button>
                  </div>
                  <div className="text-xs text-gray-500 truncate mb-1">{order.customer}</div>
                  <div className="text-xs font-mono text-gray-600 flex items-center gap-2">
                    <div className="w-full bg-gray-900 h-0.5"><div className="bg-cyan-900 h-0.5 w-full animate-pulse"></div></div>
                    DRV_{order.driverID}
                  </div>
                </div>
              ))}
            </div>
          </div>

          {/* Treap */}
          <div className="bg-[#111] border border-gray-800 flex flex-col">
            <div className="p-3 border-b border-gray-800 flex justify-between items-center">
              <h2 className="text-sm font-bold text-emerald-500 tracking-wider uppercase">Treap</h2>
              <span className="text-xs font-mono text-gray-500">n = {systemState.availableDrivers.length}</span>
            </div>
            <div className="p-4 flex-1 overflow-y-auto space-y-2">
              {systemState.availableDrivers.length === 0 && <div className="text-xs font-mono text-gray-600">{">"} TREAP_EMPTY</div>}
              {systemState.availableDrivers.map(driver => (
                <div key={driver.id} className="flex items-center justify-between border border-gray-800 bg-black p-3 hover:bg-gray-950">
                  <div>
                    <div className="font-mono text-sm text-gray-300">{driver.name}</div>
                    <div className="text-[10px] font-mono text-gray-600">ID_{driver.id}</div>
                  </div>
                  <div className="text-right">
                    <div className="text-[10px] font-mono text-gray-500 mb-0.5">SCORE</div>
                    <div className="font-mono text-emerald-500 text-sm">{driver.score}</div>
                  </div>
                </div>
              ))}
            </div>
          </div>

        </div>
      </div>
    </div>
  );
}

export default App;